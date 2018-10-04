echo -e "\n\n*** BUILD ***\n\n"
cp LICENSE r/variantkey/
cd r
$R -e 'library(devtools);library(roxygen2);setwd("./variantkey");document()'
MAKEFLAGS="PKG_CFLAGS=-O3" $R CMD INSTALL --build variantkey
