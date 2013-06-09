server: execute_make_restart
	rm execute_make_restart
	make -C src/example all
	make -C src/ all
	
eclipse: execute_make_restart
	rm execute_make_restart
	make -C bin/ all
	echo "Pronto para usar no Eclipse"
	
	#cd src/example
	#javac *.java
	#cd ..
	#java -cp example example.Server
	
restart:
	echo "Restart é banda de boiolinha"
	touch execute_make_restart
	killall rmiregistry
