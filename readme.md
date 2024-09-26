# FOSGL (Folder Structure Generation Language)

## Table of Contents
1. [Introduction](#introduction)
2. [Features](#features)
3. [Installation](#installation)
4. [Usage](#usage)
5. [Syntax and Commands](#syntax-and-commands)
6. [Examples](#examples)
7. [Advanced Usage](#advanced-usage)
8. [Troubleshooting](#troubleshooting)
9. [Contributing](#contributing)
10. [License](#license)

## Introduction

FOSGL (Folder Structure Generation Language) is a powerful XML-based scripting language designed for generating and manipulating folder structures. It provides a flexible and intuitive way to create complex directory hierarchies, manage files, and execute custom scripts, making it ideal for project scaffolding, build systems, and file organization tasks.

## Features

- XML-based syntax for easy reading and writing
- Dynamic folder and file creation
- Variable support for flexible scripting
- Conditional statements (If, Else) for decision-making
- Looping constructs (While) for repetitive tasks
- Custom tag declaration for extensibility
- Lua scripting integration for advanced operations
- Path manipulation and file operations

## Installation

 1. Clone or download the repository.

 2. Install Code::Blocks

 3. Open the project in Code::Blocks and build the executable.

 Alternately, you can download repo and build it your your way because the project doesnt have any dependencies.

## Usage

To use FOSGL, create an XML file with your desired folder structure and commands, then run it using the FOSGL executable:

```
fosgl your_script.xml [variable1 value1 variable2 value2 ...]
```

You can pass optional variables as command-line arguments, which will be available in your script.

## Syntax and Commands

FOSGL uses XML tags to define its commands. Here are the basic commands:

- `<Folder>`: Creates a new folder
- `<File>`: Creates a new file
- `<Variable>`: Defines a variable
- `<If>`: Conditional statement
- `<Else>`: Alternative for conditional statement
- `<While>`: Looping construct
- `<Script>`: Executes a Lua script
- `<Copy>`: Copies files or directories
- `<Declare>`: Defines a custom tag
- `<Custom>`: Executes a custom-defined tag

Attributes and text content within these tags provide additional information or content for the commands.

## Examples

### Basic Folder Structure

```xml
<Folder name="Root">
  <Folder name="src">
    <Folder name="components" />
    <Folder name="utils" />
  </Folder>
  <Folder name="tests" />
  <File name="README.md">
    This is a sample project.
  </File>
</Folder>
```

### Using Variables and Conditions

```xml
<Folder name="Project">
  <Variable name="create_tests" value="true" />
  <Folder name="src" />
  <If expression="$(create_tests) == 'true'">
    <Folder name="tests" />
  </If>
</Folder>
```

### Looping

```xml
<Folder name="Root">
  <Variable name="count" value="1" />
  <While expression="$(count) <= 3">
    <Folder name="folder_$(count)" />
    <Variable name="count" value="~attribute=tostring($(count)+1)" />
  </While>
</Folder>
```

## Advanced Usage

### Custom Tags with Declare

The `<Declare>` tag allows you to define custom tags for reusable operations. This is a powerful feature for creating your own abstractions and reducing repetition in your FOSGL scripts.

Syntax:
```xml
<Declare tag="TagName" attributes="attr1;attr2;..." script="Lua script">
```

Example:
```xml
<Declare tag="CreateComponent" attributes="name" script="
  local component_dir = Path.Concat(variables:GetVariable('src_path'), $(name))
  Path.MakeDir(component_dir)
  local file_path = Path.Concat(component_dir, $(name)..'.js')
  local file = io.open(file_path, 'w')
  file:write('export const '..$(name)..' = () => {\n  // Component logic here\n};')
  file:close()
"/>

<Folder name="Project">
  <Variable name="src_path" value="$(this)/src" />
  <Folder name="src">
    <CreateComponent name="Header" />
    <CreateComponent name="Footer" />
  </Folder>
</Folder>
```

In this example, we declare a custom `CreateComponent` tag that creates a new component folder and file. We can then use this custom tag just like any built-in tag in our FOSGL script.

### Lua Scripting

You can use Lua scripts for more complex operations:

```xml
<Script>
local function create_numbered_files(dir, count)
  for i = 1, count do
    local file_path = Path.Concat(dir, "file_"..i..".txt")
    local file = io.open(file_path, 'w')
    file:write("This is file number "..i)
    file:close()
  end
end

create_numbered_files(variables:GetVariable('this'), 5)
</Script>
```

## Troubleshooting

If you encounter issues:

1. Check your XML syntax for errors.
2. Ensure all referenced variables are defined.
3. Verify file paths and permissions.
4. Check the console output for error messages.

For more help, please open an issue on the GitHub repository.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.