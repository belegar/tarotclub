VERSION=$1
HG_TAG=VERSION_${VERSION//./_}

rm *.tar.gz
wget https://bitbucket.org/tarotclub/tarotclub/get/${HG_TAG}.tar.gz
mv *.tar.gz tarotclub_${VERSION}.tar.gz

