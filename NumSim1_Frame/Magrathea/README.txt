The script for building the geometry is based on Malte's version.
Build the script by typing "make" in the console.
Use the script by the instructions given after typing "magrathea --help".

The symbols in the .geom files are encoded as following
	All simulation parameters are listed and the geometry is encoded with a character map using following characters:
	' '	Fluid (Color: white, 0xFFFFFF)
	'#'	Wall/Obstacle/NoSlip (Color: black, 0x000000)
	'I'	General Inflowboarder (Ui and Vi set) (Color: magenta 0xFF00FF)
	'H'	Horizontal Inflowboarder (fluid flows in direction of Ui, and Vi and is not determined here) (Color: yellow, 0xFFFF00)
	'V'	Vertical Inflowboarder (fluid flows in direction of Vi, and Ui and is not determined here) (Color: red, 0xFF0000)
	'O'	Outflow (Color: blue, 0x0000FF)
	Use the following two boundary conditions for pressure-driven flow
	'-'	Vertical Slip-boundary (pressure is determined by "pressure") (Color: green, 0x00FF00)
	'|'	Horizontal Slip-boundary (pressure is determined by "pressure") (Color: cyan, 0x00FFFF)
