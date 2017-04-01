# HiTToP HTTP library

## Parser benchmarks

```
Hardware Overview:

  Model Name:	MacBook Pro
  Model Identifier:	MacBookPro11,5
  Processor Name:	Intel Core i7
  Processor Speed:	2.5 GHz
  Number of Processors:	1
  Total Number of Cores:	4
  L2 Cache (per Core):	256 KB
  L3 Cache:	6 MB
  Memory:	16 GB
  Boot ROM Version:	MBP114.0172.B09
  SMC Version (system):	2.30f2
  Serial Number (system):	C02RV153G8WP
  Hardware UUID:	EFB0AB9E-4E9A-538F-A056-6489287D5A7A

$ bazel-bin/hittop/http/parse_request_bench hittop/http/curl_request.bin 100000
Ok total: 60226usec rps: 1.66041e+06 usec/r: 0.60226
Ok total: 70494usec rps: 1.41856e+06 usec/r: 0.70494
Ok total: 62644usec rps: 1.59632e+06 usec/r: 0.62644
Ok total: 63899usec rps: 1.56497e+06 usec/r: 0.63899
Ok total: 71412usec rps: 1.40032e+06 usec/r: 0.71412
Ok total: 61767usec rps: 1.61899e+06 usec/r: 0.61767
Ok total: 68964usec rps: 1.45003e+06 usec/r: 0.68964
Ok total: 66560usec rps: 1.5024e+06 usec/r: 0.6656
Ok total: 69118usec rps: 1.4468e+06 usec/r: 0.69118
Ok total: 64135usec rps: 1.55921e+06 usec/r: 0.64135
$ bazel-bin/hittop/http/parse_request_bench hittop/http/chrome_request.bin 1000000
Ok total: 2.86204e+06usec rps: 349401 usec/r: 2.86204
Ok total: 2.83768e+06usec rps: 352400 usec/r: 2.83768
Ok total: 2.85374e+06usec rps: 350418 usec/r: 2.85373
Ok total: 2.85271e+06usec rps: 350543 usec/r: 2.85271
Ok total: 2.84911e+06usec rps: 350986 usec/r: 2.84911
Ok total: 2.90233e+06usec rps: 344551 usec/r: 2.90233
Ok total: 2.83469e+06usec rps: 352772 usec/r: 2.83469
Ok total: 2.86118e+06usec rps: 349506 usec/r: 2.86118
Ok total: 2.86194e+06usec rps: 349413 usec/r: 2.86194
Ok total: 2.84142e+06usec rps: 351936 usec/r: 2.84142
```
