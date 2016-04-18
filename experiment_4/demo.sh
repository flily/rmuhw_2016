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
cat <<EOF | ./contact demo.db
    insert values id=1 , name = Li Lei ,     age=12,phone=13812340001,email=lilei@example.com,resume=This is Li Lei;
EOF

cat <<EOF | ./contact demo.db
    insert values id=2 , name = Han Meimei,    age=12,phone=13512340002,email=hanmm@example.com,resume=This is Han Meimei;
EOF

cat <<EOF | ./contact demo.db
    insert values id=3 , name = Lily King ,    age=11,phone=13753573821,email=king.lily@example.com,resume=This is Lily King;
EOF
cat <<EOF | ./contact demo.db
    insert values id=4 , name = Lucy King ,     age=11,phone=13753573822,email=king.lucy@example.com,resume=This is Lucy King;
EOF
cat <<EOF | ./contact demo.db
    insert values id=5 , name = Jim Green,     age=13,phone=13524564323,email=green.jim@example.com,resume=This is Jim Green;
    insert values id=6 , name = Kate Green,     age=12,phone=13524567437,email=green.kate@example.com,resume=This is Kate Green;
EOF

echo
echo "+ show all data in demo.db"
cat <<EOF | ./contact demo.db
    select *
EOF

echo
echo "+ find King's family"
cat <<EOF | ./contact demo.db
    select * where name = King
EOF

echo
echo "+ find names of King's family"
cat <<EOF | ./contact demo.db
    select name where name = King
EOF

echo
echo "+ find Green's family"
cat <<EOF | ./contact demo.db
    select * where name = Green
EOF


echo
echo "+ find 12-year-old children"
cat <<EOF | ./contact demo.db
    select * where age = 12
EOF

echo
echo "+ find names and phones of  12-year-old children"
cat <<EOF | ./contact demo.db
    select name, phone where age = 12;
EOF

echo
echo "+ find names and phones of  15-year-old children"
cat <<EOF | ./contact demo.db
    select name, phone where age = 15;
EOF

echo
echo "+ find owner of email king.lily@example.com"
cat <<EOF | ./contact demo.db
    select * where email = king.lily@example;
EOF

