# invoke SourceDir generated makefile for helloWorld.pem4f
helloWorld.pem4f: .libraries,helloWorld.pem4f
.libraries,helloWorld.pem4f: package/cfg/helloWorld_pem4f.xdl
	$(MAKE) -f C:\Users\BrianS\workspace_v6_0\brian_g_aero1\playground\helloWorld_TivaTM4C1233H6PM/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\BrianS\workspace_v6_0\brian_g_aero1\playground\helloWorld_TivaTM4C1233H6PM/src/makefile.libs clean

