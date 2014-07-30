# invoke SourceDir generated makefile for hello.pem4f
hello.pem4f: .libraries,hello.pem4f
.libraries,hello.pem4f: package/cfg/hello_pem4f.xdl
	$(MAKE) -f C:\Users\BrianS\workspace_v6_0\brian_g_aero1\playground\hello_TivaTM4C1233H6PM/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\BrianS\workspace_v6_0\brian_g_aero1\playground\hello_TivaTM4C1233H6PM/src/makefile.libs clean

