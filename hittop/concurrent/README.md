# HiTToP Concurrency Library

Contains modern C++ concurrency primitives designed for use in architectures
making heavy use of asynchronous I/O, processing.

## OrderedActionPair, OrderedActionSequence

These primitives solve the problem where one or more actions may be ready to
perform in an unspecified order, but are must be performed in a specific order.
For example, perhaps a number of results are being calculated concurrently but
they need to be inserted into an output queue in a particular order.

For example:

```C++
// The calculation we want to perform
OutputType calculate(InputType);

// The inputs to our calculations:
std::vector<Input> inputs = GenerateInputs();

// The outputs from our calculations:
std::vector<Output> outputs;
outputs.reserve(inputs.size());

// An OrderedActionSequence that controls the insertion order of outputs.
hittop::concurrent::OrderedActionSequence sequence;

// Kick off all calculations using a thread pool.
for (const auto& in : inputs) {
  auto next_action = [in, &outputs]() {
    outputs.emplace_back(calculate(in));
  };

  auto ordered_action = sequence.WrapNext(next_action);

  thread_pool.Schedule(ordered_action);
}

// As the final action in the sequence, signal a future.
std::promise<void> p;
auto signal_done = sequence.WrapAction([&p]() { p.set_value(); });
signal_done();

std::future<void> done = last_action.get_future();
done.wait();

// At this point, the output vector is full.
```

This can be much more efficient than more general synchronization primitives
because `OrderedActionPair` and `OrderedActionSequence` use a lock-free,
wait-free algorithm to ensure proper ordering.  The rough idea is:

```
class OrderedActionPair {
  enum States {
    initial,
    run_first_calls_both,
    run_second_calls_g
  };

  atomic<State> state = initial;
  Action g;

  RunFirst(f) {
    f();
    observed_state = state.load();
    while (true) {
      if (observed_state == run_first_calls_both) {
        return g();
      } else {
        if (state.compare_exchange(observed_state, run_second_calls_g)) {
          return;
        }
      }
    }
  }

  RunSecond(g) {
    observed_state = state.load();
    this->g = g;
    while (true) {
      if (observed_state == run_second_calls_g) {
        return g();
      } else {
        if (state.compare_exchange(observed_state, run_first_calls_both)) {
          return;
        }
      }
    }
  }
}
```

The implementation of `OrderedActionSequence` works by composing
`OrderedActionPair` objects together in a kind of linked list.  The general
principle is that each action in the sequence is the first action of the next
ordered pair, run as the second action of the previous ordered pair.

(As an aside, a simple mistake in doing this composition is to reverse the
nesting, to make each ordered action in the sequence the second action of the
previous pair, run as the first action of the next pair.  This doesn't work
because running the first action of an ordered pair is always immediate; in
order words, this fails to respect the ordering constraint that each action in
the sequence be performed _strictly after_ all of the preceeding actions.)

The naive implementation of this composition introduces a suble bug/limitation
wherein a worst case invocation order of the wrapped actions in a sequence
can defer the running of all actions until the final call, essentially building
up a linked list of all the actions which is stored in the 'g' field of the
first `OrderedActionPair`.  If these are executed recursively, as they would in
the pseudo-C++ above, it would surely result in a stack overflow.

The solution is derived from observing that all calls to 'g' appear as tail
calls in the naive implementation.  Therefore, if C++ guaranteed tail-call
optimization, this wouldn't be a problem as none of those calls would grow the
stack.  As it is, we must give the compiler a little help by using
`hittop::util::TailCall`, which is essentially a zero-argument function that
returns an instance of its own type.  Wherever we have a tail-call in the
code above, we replace that by returning the unexecuted function/lambda.  Usage
of this pattern at the top level looks like:

```C++
TailCall tc = pair->RunSecondTC(...); // or RunFirstTC(...)
while (tc) {
  tc = tc();
}
```

Thus recursion is literally transformed to iteration by making the TCO
(tail-call-optimization) explicit rather than relying on the compiler to do it
for us.
