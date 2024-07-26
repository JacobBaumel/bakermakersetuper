# Bakermakersetuper (setup client)

------

## Introduction

This is the client program for [bakermaker](https://github.com/jacobbaumel/bakermaker). Its purpose is to assist in
the process of distributing git clients, ssh keys, and configuring the git environment. Read the README on the
bakermaker project to learn more about the project as a whole.

This project is licensed under the [MIT license](https://mit-license.org/).

## Restrictions

This project assumes there are no preinstalled libraries or resources available on the target machine. This project also
assumes the resulting executable will have to be distributed as a single file, and does not expect the user to have to
set up the runtime environment. To do this, this project uses [libromfs](https://github.com/werwolv/libromfs) to package
all resources into the executable. This includes a copy of the git client to be used.

## Compiling the setup program

Compilation of this project is very similar as to the parent program. Like the parent program, this project
submodules [GLFW](https://github.ccom/GLFW/GLFW), [Dear ImGui](https://github.com/ocornut/imgui),
[libromfs](https://github.com/werwolv/libromfs), and [miniz](https://github.com/richgel999/miniz) into the [libs](
./libs) folder.

Before building the project, you must provide a zipped git client to be extracted to the location chosen at runtime, 
located at [resources/PortableGit.zip](resosurces/PortableGit.zip).

## Usage
After starting the program, the user will be presented with an option to open a file picker, and a dropdown to select a
key. The resulting folder from the file picker will be the extraction location of the PortableGit.zip file, and the
dropdown will select which of the keys in the `keys/` folder in the runtime environment will be installed. The selected
key will be copied to `~/.ssh/<keyname>`, and an ssh config file will be created (or appended to if it already exists) 
with the following contents:
```
Host git
    HostName <ip>
    Port <port>
    User git
    IdentityFile ~/.ssh/<keyname>
```

Where `<ip>` and `<port>` are extracted from the first and second lines, respectively, of a file named `ip` in the
working directory of the program.

In addition, a git config file will be created (or overwritten if it already exists) at `~/.gitconfig` with the contents:
```
[user]
    name = <keyname>
    email = <keyname>
```

The runtime environment for this program is expected to be created by the parent program, not by the user.

## Reporting Bugs
Feel free to open a github issue if you find a bug, but I am not looking to expand this project much further past
where it currently is. It is designed for a specific circumstance, and I am more interested in other projects, so if
the bug is critical I will (maybe) fix it, but I most likely will not address feature requests or unusual bugs that
would not show under normal usage.