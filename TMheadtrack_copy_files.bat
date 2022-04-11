@echo off

cls


:TMheadtrack
	echo Kopiere data ordner
	mkdir C:\projekte\TMhdtr_dir\TMhdtr_src\TMhdtr\TMhdtr\TMheadtrack\data
	xcopy /s /y C:\projekte\TMhdtr_dir\TMface_src\TMface\TMfaceop\TMfaceop\data C:\projekte\TMhdtr_dir\TMhdtr_src\TMhdtr\TMhdtr\TMheadtrack\data

pause
exit