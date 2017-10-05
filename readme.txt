Aplikacja zastala napisana w srodowisku MS Visual Studio 2012.
Aby skompilowac w tym srodowisku aplikacje nalezy podpiac biblioteke opencv do projektu (nalezy równiez ustawic sciezki systemowe do opencv np. C:\opencv\build\x86\vc12\bin)
1. Wchodzimy w opcje projektu do zakladki C/C++ --> General
2. Do opcji Additional Include Directiories dodajemy sciezke C:\opencv\build\include
3. Nastepnie w zakladce Linker --> General
4. Do opcji Additional Library Directories dodajemy C:\opencv\build\x86\vc12\lib
5. Nastepnie w zakladce Linker -> Input  
6. Do opcji Additional Dependencies dodajemy ponizszy spis bibliotek

opencv_calib3d2411d.lib
opencv_core2411d.lib
opencv_features2d2411d.lib
opencv_flann2411d.lib
opencv_highgui2411d.lib
opencv_imgproc2411d.lib
opencv_ml2411d.lib
opencv_objdetect2411d.lib
opencv_photo2411d.lib
opencv_stitching2411d.lib
opencv_superres2411d.lib
opencv_ts2411d.lib
opencv_video2411d.lib
opencv_videostab2411d.lib

Uwaga podczas uruchamiania projektu moga wyswietlac sie bledy o brakujacych bibliotekach niektóre z nich dolaczone sa do projektu
w zaleznosci od systemu operacyjnego nalezy wkleic je do folderu systemowego system32 dla 32-bitowej wersji lub syswow64 dla 64-bitowej wersji systemu operacyjnego
