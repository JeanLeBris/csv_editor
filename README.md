# csv_editor

csv_editor is a terminal-based CSV file editor.

It is inspired by Vim and uses some of its commands and shortcuts.

## Installation

There are two ways to install csv_editor:
- a manual installation.
- the simplified installation.

### Manual installation

To manually install csv_editor, do the following steps:

1. Create or find a directory where you want the program to be installed and go in it with your terminal.

2. Clone csv_editor's repository with the following command:

```git clone https://github.com/JeanLeBris/csv_editor.git -b main```

This will clone the main branch of the project in a directory called 'csv_editor' inside of your current directory.

3. Go in the project's directory with the following command:

```cd csv_editor```

4. Finally, install the program with the following command:

```make compile OS=<Operating System name>```

Make sure to replace ```<Operating System name>``` with your corresponding operating system:
- Windows
- Linux

So, the command you will enter will look somewhat like this:

```make compile OS=Windows```

Wait until the operations stop happening and the program is installed.

### Simplified Installation

The process of the simplified installation is as follows:

1. Create or find a directory where you want the program to be installed and go in it with your terminal. (same as the manual installation)

2. You will see in my GitHub repositories a project called 'installer', we will clone it with the following command:

```git clone https://github.com/JeanLeBris/installer.git -b main```

3. Then you will enter the directory 'installer' that has just been created with the following command:

```cd installer```

4. You will then clone and install csv_editor with the following commands:

```
python installer.py clone csv_editor
python installer.py install csv_editor
```

So, to make it short, the whole command to install csv_editor is:

```
git clone https://github.com/JeanLeBris/installer.git -b main
cd installer
python installer.py clone csv_editor
python installer.py install csv_editor
cd ..
```

## Usage

```csv_editor.exe [-i | --input] file.csv``` : Open file.csv

```csv_editor.exe [-i | --input] file.csv [-sep | --separator] ";"``` : Open file.csv with ';' as the separator value between the cells

```csv_editor.exe [-i | --input] file.csv --show-config``` : Display the configuration of the program

## Configuration

csv_editor offers a variety of possibilities for customization in 'csv_editor.conf', the configuration file of csv_editor, that you can find in the installation directory of csv_editor.

### General customization

```table_type = [static | dynamic]``` : Currently this setting does nothing as the table will always be loaded statically.

```file_line_max_length = 1000``` : Defines the max character length a line in the original CSV file can be.

```file_max_length = -1``` : Defines the max length the original CSV file can be (-1 means no specific max length, it will keep reading the file until all of it is read)

```unfocused_cell_max_width = 15``` : Defines the max character width of a column you are not over.

```focused_cell_max_width = 30``` : Defines the max character width of a column you are over.

```cell_max_width = 100``` : Defines the max amount of characters that can be stored in a cell of the table.

```csv_default_separator = ,``` : Defines the character to look for as a separator in the original CSV file.

```commands_history_length = 100``` : Defines the maximum amount of previous commands that can be kept in memory.

### Color customization

Color codes :
- 3x : Dark text color
- 4x : Dark background color
- 9x : Bright text color
- 10x : Bright background color
- x0 : Black
- x1 : Red
- x2 : Green
- x3 : Yellow
- x4 : Blue
- x5 : Magenta
- x6 : Cyan
- x7 : White

```default_background_color = 40``` : Sets the default color of the background.

```default_text_color = 37``` : Sets the default color of the text.

```background_header_color = 40``` : Sets the default color of the background of the header.

```text_header_color = 34``` : Sets the default color of the text of the background.

```odd_background_line_color = 40``` : Sets the default color of the background for odd lines.

```odd_text_line_color = 37``` : Sets the default color of the text for odd lines.

```even_background_line_color = 100``` : Sets the default color of the background for even lines.

```even_text_line_color = 37``` : Sets the default color of the text for even lines.

```selection_background_color = 47``` : Sets the background color in selection areas.

```selection_text_color = 30``` : Sets the text color in selection areas.

## In the program