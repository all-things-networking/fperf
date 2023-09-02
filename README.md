# fperf

[Formal Methods for Network Performance Analysis](https://mina.arashloo.net/docs/fperf.pdf), Mina Tahmasbi Arashloo, Ryan Beckett, Rachit Agarwal, NSDI'23



The stand-alone priority schedulers, composition of schedulers, and throughput analysis on a leaf-spine topology. More to come soon!

## Installation
fperf currently runs on Z3 v4.8.11 [link](https://github.com/Z3Prover/z3/releases/tag/z3-4.8.11)

### Windows with Visual Studio
1. Download fperf through github
2. Create a new project in Visual Studio in the fperf folder
3. Download Z3 for windows [here](https://github.com/Z3Prover/z3/releases/tag/z3-4.8.11)
4. Build Z3 with the instructions from [here](https://github.com/exercism/z3/blob/main/docs/INSTALLATION.md) under **Building Z3 on Windows using Visual Studio Command Prompt**
5. Go to "Project" -> "Properties" and in the General tab and perform the following:
	- Set "Platform Toolset" to "LLVM"
	- Set "C++ Language Standard" to "C++ 17 Standard"
6. Under the same "Configuration Properties" page, go to "C/C++" -> "General" -> "Include Additional Directories" and add the following:
	- `[path to z3]\z3\src\api`
	- `[path to z3]\z3\src\api\c++`<br>
	  The following may be required if Visual Studio doesn't recognize the subfolders of fperf
	- `[path to fperf]\fperf\src\qms`
	- `[path to fperf]\fperf\src\metrics`
	- `[path to fperf]\fperf\src\cps`
	- `[path to fperf]\fperf\src`
	- `[path to fperf]\fperf\lib\cps`
	- `[path to fperf]\fperf\lib\metrics`
	- `[path to fperf]\fperf\lib\qms`
	- `[path to fperf]\fperf\lib`
7. Under "Linker" -> "General" -> "Include Additional Directories", add `[path to z3]\z3\build\`
8. Under "Linker" -> "Input", add `[path to z3]\z3\build\libz3.lib`
9. Press the green arrow in the top bar of Visual Studio to run fperf
