macro( create_library libname in_short )

	add_library( ${libname} "src/${in_short}.cc" )
	target_include_directories( ${libname} PUBLIC "src" )

endmacro()