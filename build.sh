printf 'building web'
cd web
make clean
make
cd -

printf 'building FTP'
cd FTP

printf 'building FTP server'
cd server
make clean
make
cd -

printf 'building FTP client'
cd client
make clean
make
cd -

cd -

printf 'building completed'