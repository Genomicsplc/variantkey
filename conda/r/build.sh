echo -e "\n\n*** BUILD ***\n\n"
cp LICENSE r/variantkey/
cd r
$R -e 'library(devtools);library(roxygen2);setwd("./variantkey");document()'
$R CMD INSTALL --build variantkey
