########################################
### TMhdtr Beschreibung
###  ######          C:\projekte\Mitarbeiter\mkania\TMhdtr\TMheadtrack\TMhdtr_readme.txt
########################################
- Quadrate Methode
    - Es gibt zwei mögliche Quadrate Blau und Grün
    	- TMheadtrack_color.ini  - später in der normalen Ini Datei integriert
		- um zu wechsel zwischen den Farben 
			- die Farbe in TMheadtrack_color.ini  unkomentieren
	- TMheadtrack_color.ini wird später gelöscht und die Variablen ind in TMheadtrack_config.ini eingebaut
			##### 	für 	tmheadtrack_tracking_method=1 	HSV methode, methode mit dem blauen Quadrat
			########## HSV Color Format https://de.wikipedia.org/wiki/HSV-Farbraum
			########## Grün 	
			##tmheadtrack_tracking_quadrat_color_lh=46
			##tmheadtrack_tracking_quadrat_color_ls=35
			##tmheadtrack_tracking_quadrat_color_lv=75
			##tmheadtrack_tracking_quadrat_color_uh=90
			##tmheadtrack_tracking_quadrat_color_us=255
			##tmheadtrack_tracking_quadrat_color_uv=255
			##########Blau
			tmheadtrack_tracking_quadrat_color_lh=106
			tmheadtrack_tracking_quadrat_color_ls=72
			tmheadtrack_tracking_quadrat_color_lv=142
			tmheadtrack_tracking_quadrat_color_uh=128
			tmheadtrack_tracking_quadrat_color_us=255
			tmheadtrack_tracking_quadrat_color_uv=255

    - bei Quadrat Methode ist der Mund-auf Mund-zu Maus Klick noch nicht zu Ende implementiert
    - bei Quadrat Methode ist der Intervall Klick implementiert


    - RGB Farben der Quadrate 
		R,  G, B
	green:  76,177,34
	blue:  204, 72,63
		- Quadrate müssen 25mmx25mm groß sein


- Nasepunkt Methode
     - bei Nasenpunkt Methode ist der Mund-auf Mund-zu Maus Klick implementiert
     - bei Nasenpunkt Methode ist der Intervall Klick implementiert


- Mund-auf Mund-zu Mausklick-Methode,   Intervall Mausklick Methode
    - tmheadtrack_mouse_click_method Variable
	- um die Mausklickmethode zu wechseln im TMheadtrack_config.ini oder TMheadtrack_config_user.ini umschalten
		###### TMheadtr Maus Klick Methode. 
		###### 		tmheadtrack_mouse_click_method=0 	Keine methode - Kein Mouse Click
		###### 		tmheadtrack_mouse_click_method=1 	Interval Maus Klick 
		###### 		tmheadtrack_mouse_click_method=2 	Mund Maus Klick 


	- Intervall Mausklick Methode
		- wenn man xx Sec auf einem Symbol-Button bleibt, wird auttomatisch ein Links-Mausklick erzeugt
			- Interval-Zeit-Automatisch = 0  
			- Intervallzeit=1  vorgeben wieviele Sekunden Wartezeit
		- Die Intervall-Zeit - Vorgeben in der ini-datei in mSekunden
	- Mund-auf Mund-zu Mausklick-Methode
		- wenn man xx Sec auf einem Symbol-Button bleibt, wird automatisch ein Links-Mausklick erzeugt
		- wenn man Mund-auf Mund-zu (über xx Sec) auf einem Symbol-Button macht, wird ein Links-Mausklick erzeugt
		- wenn man bei Mund-auf Mund-zu, länger (über xx Sec) Mund-auf hat, über einem Symbol-Button macht, wird ein Doppel-Links-Mausklick erzeugt



- TMheadtrack.exe kann jetzt alleine starten, ohne tmhdtr-gui.exe vorher gestartet zu haben




########################################
### TMhdtr.exe
### DevelopmentTree: C:\projekte\Mitarbeiter\mkania\TMhdtr\TMheadtrack
### DevelopmentTree: C:\Program Files\TMND-GMBH\TMhdtr
########################################

### TMheadtrack.exe Datai
	- kopiere von 	C:\Users\tmnd_2\Desktop\KaniaM\Tastatur mit Kopf bewegung C++\Tastatur mit Kopf bewegung C++\x64\Release 
		  nach  C:\Program Files\TMND-GMBH\TMhdtr

### Downloaden und unzipen von opencv Visual studio 16 2019	opencv-4.1.0-vc16.zip
	- https://www.dropbox.com/s/2aqfcmg529jrlpk/opencv-4.1.0-vc16.zip?dl=1
	- opencv  Dateien xxx.exe, xxx.dll, xxxd.dll, _dlib-lib in verschiedene Verzeichnisse trennen
	    	in C:\Program Files\TMND-GMBH\TMhdtr\__downloads\20200906_opencv\
			- _dlib-lib
			- _opencv-exe
			- _opencv-dll
			- _opencv-d-dll

### Kopiere opencv-dll ohne Debug in Runtime Tree von  
	von  		C:\Program Files\TMND-GMBH\TMhdtr\__downloads\20200906_opencv\_opencv-dll 
	nach 		C:\Program Files\TMND-GMBH\TMhdtr

### Download von dlib.lib von
	von  		C:\dlib-19.6\build\dlib\Release C:\Program Files\TMND-GMBH\TMtrack
	nach 		C:\Program Files\TMND-GMBH\TMhdtr\__downloads\20200906_opencv\_dlib-lib

### Kopiere von dlib.lib  ohne Debug in Runtime Tree 
	von  		C:\Program Files\TMND-GMBH\TMhdtr\__downloads\20200906_opencv\_dlib-lib
	nach 		C:\Program Files\TMND-GMBH\TMhdtr
		
### install die Visual Studio 2015, 2017 and 2019 Runtime Version  vc_redist.x64.exe = VC_redist.x64_VS2019_redistr_TMhdtr.exe
### für DLLs die fählen wie: 	VCRUNTIME140_1.dll 
	Download:  	https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads
			https://aka.ms/vs/16/release/vc_redist.x64.exe
	Kopiere: 	C:\Program Files\TMND-GMBH\TMhdtr\__downloads\20200914_VS2019-Redistr
	Installiere	C:\Program Files\TMND-GMBH\TMhdtr\__downloads\20200914_VS2019-Redistr\VC_redist.x64_VS2019_redistr_TMhdtr.exe


###  TMhdtr_face_tracking_02.dat  - Original:  shape_predictor_68_face_landmarks.dat 
	Download:  	https://github.com/AKSHAYUBHAT/TensorFace/raw/master/openface/models/dlib/shape_predictor_68_face_landmarks.dat 
	Kopiere	   	C:\Program Files\TMND-GMBH\TMhdtr\__downloads\20200914_TMhdtr_face_tracking_02


###  Kopiere TMhdtr_face_tracking_02.dat  
	von   		C:\Program Files\TMND-GMBH\TMhdtr\__downloads\20200914_TMhdtr_face_tracking_02
	nach 		C:\Program Files\TMND-GMBH\TMhdtr


### antivir quarantäne herstellen
### https://support.avira.com/hc/de/articles/360003070838-Dateien-in-Quarant%C3%A4ne-wiederherstellen-oder-l%C3%B6schen#:~:text=Wurde%20eine%20Datei%20f%C3%A4lschlicherweise%20als,pr%C3%BCfen%2C%20Wiederherstellen%20oder%20L%C3%B6schen%20wollen.
	Sie benötigen Berechtigungen zur Durchführung des Vorgangs




########################################
### TMhdtr-gui.exe
### DevelopmentTree: C:\projekte\Mitarbeiter\mkania\TMhdtr-gui
### DevelopmentTree: C:\Program Files\TMND-GMBH\TMhdtr
########################################
Knöpfe Beschreibung:
	-Starte Program mit Camera - TMhdtr startet mit camerabild Fenstern
	-Starte Program ohne Camera - TMhdtr startet ohne camerabild Fenstern
	-Version wählen - wähle mit welcher Metode TMtrack startet. Wen das program erstes Mal gestartet wird von TMhdtr-gui
	 muss man erstmal die version wählen sonst wird eine Fehlermeldung fur den Benutzer gezeigt 
	-Standardeinstellung - löscht die TMheadtrack_config_user.ini damit das Programm nur die Standard Einstellungen von
	 TMheadtrack_config.ini liest
	-Beenden - schließt TMhdtr-gui

Das gui hat zwei Tabs. Im tab -version gibt's ein zurück Knopf aber man kann auch die andere tab manuel auswählen

WICHTIG!!!  - damit das program startet muss die TMhdtr-gui.exe und TMheadtrack.exe in der gleichen dir sein