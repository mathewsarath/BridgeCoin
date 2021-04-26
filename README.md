# BridgeCoin
Note: This is for educational purposes, showing how to develop a cryptocurrency using C++20; it is not a real cryptocurrency.
I wanted a new project to teach and also improve my own knowledge about C++20. This is combined with my love for cryptocurrency  solving real-world problems that are in existing ones. Common problems are either the language chosen for the reference implementation are not performant enough e.g using GoLang (eth/avax), have fees (most cryptos) and most are not designed with performance in mind from the get-go and try to shoe horn optimizations in afterwards. People often quote VISA as peaking over 57K tps, crypto opens up a lot more doors for new innovation and most of the world does not even has access to this network. I believe over 1Million TPS is required for a fully automonous world.

Features include:  
* Decentralized
* Instant - Finality will need to be reached very quickly
* Consensus mechanism - Novel
* Smart contracts
* User friendly - Requiring the underlying crypto to pay as a gas fee for interacting with smart contracts is very bad UX and makes onboarding users much more difficult (imagine scheduling nationwide voting system).
* DB driven design
* No serialization/deserialization required and many other features making use of the underling hardware architecture.
* Signature verification is moved outside to reduce compute load
* Sharding of the network to improve throughput

![bridge](https://user-images.githubusercontent.com/650038/115217114-138b5300-a0fd-11eb-983d-4da1eff1aa46.png)

## Getting Started

## Dependencies

Note about install commands: 
- for Windows, we use [choco](https://chocolatey.org/install).
- for MacOS, we use [brew](https://brew.sh/). 
- In case of an error in cmake, make sure that the dependencies are on the PATH.

### Necessary Dependencies
1. A C++ compiler that supports C++20. See [cppreference.com](https://en.cppreference.com/w/cpp/compiler_support) to see which features are supported by each compiler.
The following compilers should work:

  * [gcc 11+](https://gcc.gnu.org/)
	<details>
	<summary>Install command</summary>

	- Debian/Ubuntu:

			sudo apt install build-essential

	- MacOS:
			
			brew install gcc
	</details>

  * [clang 11+](https://clang.llvm.org/)
	<details>
	<summary>Install command</summary>

	- Debian/Ubuntu:
		
			bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"

	- Windows:

		Visual Studio 2019 ships with LLVM (see the Visual Studio section). However, to install LLVM separately:
		
			choco install llvm -y
			
		llvm-utils for using external LLVM with Visual Studio generator:
			
			git clone https://github.com/zufuliu/llvm-utils.git
			cd llvm-utils/VS2017
			.\install.bat

	- MacOS:
 		
			brew install llvm
	</details>

  * [Visual Studio 2019 or higher](https://visualstudio.microsoft.com/)
	<details>
	<summary>Install command + Environment setup</summary>

	On Windows, you need to install Visual Studio 2019 because of the SDK and libraries that ship with it.

  	Visual Studio IDE - 2019 Community (installs Clang too):
		
  	  	choco install -y visualstudio2019community --package-parameters "add Microsoft.VisualStudio.Workload.NativeDesktop --includeRecommended --includeOptional --passive --locale en-US"
		
	Put MSVC compiler, Clang compiler, and vcvarsall.bat on the path:

			choco install vswhere -y
			refreshenv
			
			$clpath = vswhere -latest -prerelease -find **/Hostx64/x64/*   # for x64
			[Environment]::SetEnvironmentVariable("Path", $env:Path + ";$clpath", "User")
			
			$clangpath = vswhere -latest -prerelease -find **/Llvm/bin/*
			[Environment]::SetEnvironmentVariable("Path", $env:Path + ";$clangpath", "User")

			$vcvarsallpath =  vswhere -latest -prerelease -find **/Auxiliary/Build/*
			[Environment]::SetEnvironmentVariable("Path", $env:Path + ";$vcvarsallpath", "User")
			refreshenv

	</details>


2. [Conan](https://conan.io/)
	<details>
	<summary>Install Command</summary>
	
	- Via pip - https://docs.conan.io/en/latest/installation.html#install-with-pip-recommended
		
			pip install --user conan
	
	- Windows:
		
			choco install conan -y
	
	- MacOS:
	 	
			brew install conan
	
	</details>

3. [CMake 3.15+](https://cmake.org/)
	<details>
	<summary>Install Command</summary>
	
	- Debian/Ubuntu:
		
			sudo apt-get install cmake
	
	- Windows:
		
			choco install cmake -y
	
	- MacOS:
	 		
			brew install cmake
	
	</details>

### Optional Dependencies
#### C++ Tools
  * [Doxygen](http://doxygen.nl/)
	<details>
	<summary>Install Command</summary>

	- Debian/Ubuntu:
		
			sudo apt-get install doxygen
			sudo apt-get install graphviz

	- Windows:
		
			choco install doxygen.install -y
			choco install graphviz -y

	- MacOS:
 		
			brew install doxygen
	 		brew install graphviz

	</details>

## Build Instructions

### Build directory
Make a build directory:
```
mkdir build
cd build
```
### Specify the compiler using environment variables

By default (if you don't set environment variables `CC` and `CXX`), the system default compiler will be used.

Conan and CMake use the environment variables CC and CXX to decide which compiler to use. So to avoid the conflict issues only specify the compilers using these variables.

CMake will detect which compiler was used to build each of the Conan targets. If you build all of your Conan targets with one compiler, and then build your CMake targets with a different compiler, the project may fail to build.

<details>
<summary>Commands for setting the compilers </summary>

- Debian/Ubuntu/MacOS:
	
	Set your desired compiler (`clang`, `gcc`, etc):
		
	- Temporarily (only for the current shell)
	
		Run one of the followings in the terminal:
	
		- clang
		
				CC=clang CXX=clang++
			
		- gcc
		
				CC=gcc CXX=g++
	
	- Permanent:

		Open `~/.bashrc` using your text editor:
			
			gedit ~/.bashrc
			
		Add `CC` and `CXX` to point to the compilers:
			
			export CC=clang
			export CXX=clang++
			
		Save and close the file.

- Windows:

	- Permanent:
	
		Run one of the followings in PowerShell:
				
		- Visual Studio generator and compiler (cl)
			
				[Environment]::SetEnvironmentVariable("CC", "cl.exe", "User")
				[Environment]::SetEnvironmentVariable("CXX", "cl.exe", "User")
				refreshenv
			
		  Set the architecture using [vsvarsall](https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2019#vcvarsall-syntax):
			
				vsvarsall.bat x64

		- clang

				[Environment]::SetEnvironmentVariable("CC", "clang.exe", "User")
				[Environment]::SetEnvironmentVariable("CXX", "clang++.exe", "User")
				refreshenv
	 
		- gcc

				[Environment]::SetEnvironmentVariable("CC", "gcc.exe", "User")
				[Environment]::SetEnvironmentVariable("CXX", "g++.exe", "User")
				refreshenv
	 

  - Temporarily (only for the current shell):
		
			$Env:CC="clang.exe"
			$Env:CXX="clang++.exe"
			
</details>

### Configure your build
#### cmake-command line
cmake ..

#### cmake-gui (coming soon)

#### **Configure via ccmake**:
with the CMake Curses Dialog Command Line tool:  

    ccmake -S . -B ./build

Once `ccmake` has finished setting up, press 'c' to configure the project, 
press 'g' to generate, and 'q' to quit.

### Build
Once you have selected all the options you would like to use, you can build the 
project (all targets):

    cmake --build ./build

For Visual Studio, give the build configuration (Release, RelWithDeb, Debug, etc) like the following:

    cmake --build ./build -- /p:configuration=Release

## Troubleshooting

### Update Conan
Many problems that users have can be resolved by updating Conan, so if you are 
having any trouble with this project, you should start by doing that.

To update conan: 

    $ pip install --user --upgrade conan 

You may need to use `pip3` instead of `pip` in this command, depending on your 
platform.

### Clear Conan cache
If you continue to have trouble with your Conan dependencies, you can try 
clearing your Conan cache:

    $ conan remove -f '*'
    
The next time you run `cmake` or `cmake --build`, your Conan dependencies will
be rebuilt. If you aren't using your system's default compiler, don't forget to 
set the CC, CXX, CMAKE_C_COMPILER, and CMAKE_CXX_COMPILER variables, as 
described in the 'Build using an alternate compiler' section above.