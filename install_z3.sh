Z3_NAME=z3-4.8.11-x64-glibc-2.31
apt install -y build-essential cmake unzip
wget -O /tmp/z3.zip https://github.com/Z3Prover/z3/releases/download/z3-4.8.11/$Z3_NAME.zip
unzip z3.zip
cd "/tmp/$Z3_NAME"
cp bin/libz3.so /usr/local/lib
cp include/* /usr/local/include


