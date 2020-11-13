Dieser Ordner beinhaltet ein Skelett für einen SPL-Compiler in C.
Im Laufe der 6 Praktikumsaufgaben stellen Sie diesen SPL-Compiler fertig.
Dafür enthält dieses Dokument einige wichtige Hinweise!
Lesen Sie auch das nicht sprachspezifische README, das oberhalb dieses Ordners liegt.


1. Die Ordnerstruktur

Dieser Ordner enthält einen Unterordner 'src'.
Er enthält sowohl die Header- als auch die Quelldateien des Skeletts in einzelnen Unterordnern.
Beachten Sie, dass Sie in der Regel nur die Dateien des 'phases' Unterordners ergänzen müssen.
Alle anderen Dateien beinhalten ausschließlich Librarycode, den Sie benutzen werden.
Die Dateien und Ordner, und ab welcher Aufgabe sie relevant werden, sind im unteren Diagramm kurz beschrieben.
Für Details zu den einzelnen Quelldateien schauen Sie sich bitte die Dokumentation innerhalb der Datei an.

src/
    phases/                 -- Alle Dateien, die von Ihnen ergänzt werden müssen. Die Namen der Unterordner geben das zugehörige Aufgabenblatt an.
        /_01_scanner/       -- Die Definition des Scanners (PA1)
        /_02_03_parser      -- Die Definition des Parsers (PA2 und PA3)
        /_04a_tablebuild    -- Der Code für die Konstruktion der Symboltabellen (PA4 Teil A)
        /_04b_semant        -- Der Code für die semantische Prüfung der Prozedurrümpfe (PA4 Teil B)
        /_05_varalloc       -- Der Code für die Festlegung von Speicherplätzen in Stackframes (PA5)
        /_06_codegen        -- Der Code für die Eco32-Assemblercodegenerierung (PA6)
    /absyn                  -- Die Definitionen des abstrakten Syntaxbaums (AST). Ab PA3
    /table                  -- Die Definitionen der Symboltabelle und ihrer Einträge. Ab PA4 Teil A
    /types                  -- Die Definitionen von SPL-Typgraphen. Ab PA4 Teil A
    /util                   
        /errors.[c|h]       -- Die Definitionen verschiedener Fehlerfälle, die während des Compileprozesses auftreten können, mit ihren Exit-Codes. Immer relevant, besonders in PA4
        /memory.[c|h]       -- Kleine Hilfsfunktionen zur sichereren Verwaltung von Speicher. Für Sie i.A. nicht relevant.
.gitignore      -- Eine Datei, die der Versionsverwaltung die Dateien auflistet, die ignoriert werden, also nicht dem Repository hinzugefügt werden sollen (z.B. IDE Dateien, Builddateien).
CMakeLists.txt  -- Die Projektdatei für das Buildsystem CMake. 


2. Softwarevoraussetzungen

Das Projekt nutzt das CMake-Buildsystem in der Version 3.10. CMake ist ein plattformunabhängiger "Buildfile-Generator". Von unserer Seite aus wird allerdings nur Linux unterstützt.
Für die Windowsnutzer: Wir empfehlen sehr stark, das Windows Subsystem for Linux zu installieren und einzurichten! Mehr dazu erfahren Sie im README, das oberhalb dieses Ordners liegt.

CMake nutzt eine MakeLists.txt-Datei, die die Inhalte des Projekts beschreibt. Diese ist bereits in diesem Ordner enthalten.
CMake funktioniert so, dass der Aufruf von cmake nicht direkt das Projekt baut, sondern stattdessen Builddateien für die aktuelle Plattform generiert.
Unter Linux wird zum Beispiel eine Makefile generiert, unter Windows Projektdateien für das MSBuild-System.
Mehr über CMake erfahren Sie auf cmake.org.

Um das Projekt bauen zu können brauchen Sie neben CMake einen C11-fähigen Compiler, sowie den Scannergenerator flex, und den Parsergenerator bison.
Die allermeisten Linux Distributionen werden mit gcc in einer C11-kompatiblen Version ausgeliefert. Die erwähnten Generatoren müssen sie manuell nachinstallieren.

Unter Ubuntu können Sie alle Voraussetzungen mit dem folgenden Befehl installieren:
    sudo apt install cmake flex bison


3. IDE-Nutzung

Wir empfehlen eine IDE zu nutzen. Dafür empfehlen wir CLion von Jetbrains. CLion basiert auf der IntelliJ Plattform, die Sie ja bereits kennen sollten.
Wie für IntelliJ Sie auch für CLion als Student eine kostenlose Lizenz.

Wichtig: Öffnen Sie mit CLion den Ordner, in dem dieses README und die CMakeLists.txt liegen. Also der Ordner mit dem Namen 'c'.
         Öffnen Sie NICHT den Überordner oder den src-Unterordner. Nur so versteht CLion Ihr Projekt und kann normal genutzt werden!

Windows 10: CLion funktioniert unter Windows 10 am leichtesten mit der WSL Toolchain. Dafür müssen Sie einen ssh-Zugang einrichten, mit dem CLion
            auf das Subsystem zugreifen kann. Eine Anleitung von Jetbrains dazu finden Sie hier: https://www.jetbrains.com/help/clion/how-to-use-wsl-development-environment-in-clion.html

Der Buildprozess kann unter CLion unter Umständen Probleme machen und erst beim dritten Versuch funktionieren. 
Der Grund dafür ist, dass CMake versucht einzelne Aufgaben zu parallelisieren, die aber voneinander abhängen.
Um dies zu verhindern öffnen Sie in CLion das Menü Settings -> Build, Execution, Deployment -> CMake.
Fügen Sie dort unter 'Build options' den Wert '-j 1' ein. Dadurch führt CMake maximal eine Aufgabe gleichzeitig aus.


4. Manuelles Bauen und Ausführen.

Wenn Sie das Projekt manuell bauen wollen, erstellen Sie einen zusätzlichen Unterordner 'build'.
Öffnen Sie in diesem Ordner ein Terminal und führen Sie die folgenden Befehle aus:
    cmake ..
    make

Dieser Prozess wird auch 'Out of source build' genannt, da die Builddateien nicht im gleichen Ordner wie das restliche Projekt liegen.
Der CMake Aufruf generiert die Dateien für das Buildsystem 'make'. Der anschließende Aufruf baut dann tatsächlich das Projekt.
Anschließend sollte, wenn der Kompilierprozess nicht fehlschlägt, im Buildordner eine Datei 'spl' entstanden sein.
Dies ist die fertige ausführbare Datei, die Sie wie jedes andere Programm über ein Terminal starten können.

CMake unterstützt keine eingebaute Funktion, die Builddateien aufzuräumen ('clean').
Wenn Sie alle Dateien des Buildprozesses loswerden wollen, führen Sie im 'build' Ordner zunächst
    make clean
aus. Dadurch werden alle Dateien, die durch den Scanner- und Parsergenerator generiert werden, gelöscht. 
Anschließend können Sie den 'build' Ordner komplett löschen.
