echo -e "\n\n*** BUILD ***\n\n"
find ${SYS_PREFIX}/pkgs/ -type f -path '*/lib/R/etc/Makeconf' -exec sed -i "s/-O2/-O3/g" {} \;
cp LICENSE r/variantkey/
cd r
$R -e 'library(devtools);library(roxygen2);setwd("./variantkey");document()'
$R CMD INSTALL --build variantkey
