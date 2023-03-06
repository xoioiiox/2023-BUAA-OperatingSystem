
mkdir mydir
chmod +rwx mydir
touch myfile
echo 2023 > myfile
mv moveme ./mydir
cp copyme ./mydir
mv ./mydir/copyme ./mydir/copied
cat readme
gcc bad.c 2> err.txt
mkdir gen
if [ $# -ne 0 ]
then
	a=$1
elif [ $# -eq 0 ] 
then
	a=10
fi
while [ $a -ne 0 ]
do
	touch ./gen/$a.txt
	a=$[$a-1]
done
