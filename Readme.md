# nooks - Navigate to Saved Directories with Ease!
Effortlessly save and navigate to your favorite directories on any Unix-like system!

## Description
nooks is a sleek and powerful utility that allows you to save and navigate to your most-used directories with lightning speed! 🌩 

Say goodbye to lengthy `cd` commands and hello to streamlined directory navigation! 🎉

## Features
🌋 Muscle Memory Magic! 💪

Built with the mighty C language, nooks harnesses the raw power of your CPU! Navigate to your saved spots with a single command! 🏃‍♂️💨

🌋 Effortless Saving! 💾

Save your current directory as a default spot or give it a custom name! nooks makes it a breeze to remember your favorite locations! 😄

🌋 All-Seeing Eye! 👁

Unleash the power of `--all` and witness the grand tapestry of your saved spots! nooks lays them all out for you! 🌈

🌋 Comprehensive Help option! 🆘

Lost in the vast expanse of directories? Just hit up `--help` and nooks will light the way with its comprehensive guide! 🔦

🌋 Universal Compatibility! 🌍

From Linux to macOS to BSD, nooks feels right at home on any Unix-like system! It's the globe-trotting adventurer of directory navigation! 🗺

🌋 Actively Maintained Open-Source Powerhouse! 💪

Crafted with love and care, nooks is an open-source dynamo that just keeps getting better! 🚀

## Getting Started

### Dependencies
nooks is a self-contained, pure C code that stands tall without the need for external libraries! 🏰

### Installing
Set of steps for installation:

```
git clone https://github.com/yellow-footed-honeyguide/nooks.git 
cd nooks
mkdir build
cd build
cmake ..
make
```

Now you can move nooks to /usr/local/bin/
```
sudo make install
```



### Examples of usage
```
$ nooks -s work         # Save the current directory with the mark 'work'
$ nooks work            # Navigate to the directory saved as 'work'
$ nooks -a              # Show all saved spots
```


### Executing program
![usage-example](assets/nooks.gif)

## Authors
Contributors names and contact info

ex. Sergey Veneckiy 

mail: s.venetsky@gmail.com

## License
This project is licensed under the MIT License


