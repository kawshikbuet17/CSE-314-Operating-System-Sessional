cp -r Fresh-Copy/xv6-public/ .
echo "copied a fresh xv6 copy"
cd xv6-public/
echo "go to xv6 copy"
git status
echo "checked git status (working tree should be clean)"
make
echo "make done"
make qemu-nox

git apply ../1705043.patch
echo "patch applied"
make qemu-nox

