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

$ bazel-bin/hittop/http/parse_request_bench hittop/http/curl_request.bin 1000000
Ok total: 641093usec rps: 1.55984e+06 usec/r: 0.641093
Ok total: 610716usec rps: 1.63742e+06 usec/r: 0.610716
Ok total: 615499usec rps: 1.6247e+06 usec/r: 0.615499
Ok total: 602078usec rps: 1.66091e+06 usec/r: 0.602078
Ok total: 607252usec rps: 1.64676e+06 usec/r: 0.607252
Ok total: 621470usec rps: 1.60909e+06 usec/r: 0.62147
Ok total: 625602usec rps: 1.59846e+06 usec/r: 0.625602
Ok total: 609310usec rps: 1.6412e+06 usec/r: 0.60931
Ok total: 627044usec rps: 1.59478e+06 usec/r: 0.627044
Ok total: 619486usec rps: 1.61424e+06 usec/r: 0.619486
$ bazel-bin/hittop/http/parse_request_bench hittop/http/chrome_request.bin 1000000
Ok total: 3.00333e+06usec rps: 332964 usec/r: 3.00333
Ok total: 2.97469e+06usec rps: 336169 usec/r: 2.97469
Ok total: 2.92621e+06usec rps: 341739 usec/r: 2.92621
Ok total: 2.98293e+06usec rps: 335241 usec/r: 2.98293
Ok total: 2.97598e+06usec rps: 336024 usec/r: 2.97598
Ok total: 2.9591e+06usec rps: 337940 usec/r: 2.9591
Ok total: 2.95619e+06usec rps: 338273 usec/r: 2.95619
Ok total: 2.99966e+06usec rps: 333371 usec/r: 2.99966
Ok total: 2.98728e+06usec rps: 334753 usec/r: 2.98728
Ok total: 2.96984e+06usec rps: 336719 usec/r: 2.96984
```
