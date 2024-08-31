cd estufa-esp32/;
# rm -rf build/;
. $HOME/esp/esp-idf/export.sh;
# idf.py reconfigure;
idf.py build;
idf.py flash;
idf.py monitor