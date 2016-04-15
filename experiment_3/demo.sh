#!/bin/sh

echo
echo "+ clean builds"
make clean

echo
echo "+ build"
make

echo
echo "+ remove old database file"
rm -f demo.db

echo
echo "+ add records"
./contact demo.db add --id=1 --name='Li Lei'       --age=12 --phone=13812340001 --email=lilei@example.com       --resume='This is Li Lei'
./contact demo.db add --id=2 --name='Han Meimei'   --age=12 --phone=13512340002 --email=hanmmi@example.com      --resume='This is Han Meimei'
./contact demo.db add --id=3 --name='Lily King'    --age=11 --phone=13753573821 --email=king.lily@example.com   --resume='This is Lily King'
./contact demo.db add --id=4 --name='Lucy King'    --age=11 --phone=13753573822 --email=king.lucy@example.com   --resume='This is Lucy King'
./contact demo.db add --id=5 --name='Jim Green'    --age=13 --phone=13524564323 --email=green.jim@example.com   --resume='This is Jim Green'
./contact demo.db add --id=6 --name='Kate Green'   --age=12 --phone=13524567437 --email=green.kate@example.com  --resume='This is Kate Green'

echo
echo "+ show all data in demo.db"
./contact demo.db show

echo
echo "+ find King's family"
./contact demo.db find --name=King

echo
echo "+ find Green's family"
./contact demo.db find --name=Green


echo
echo "+ find 12-year-old children"
./contact demo.db find --age=12

echo
echo "+ find 15-year-old children"
./contact demo.db find --age=15

echo
echo "+ find owner of email king.lily@example.com"
./contact demo.db find --email=king.lily@example.com
