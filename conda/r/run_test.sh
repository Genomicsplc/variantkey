echo -e "\n\n*** TEST ***\n\n"
cd ../work/r
$R -e 'library(testthat);testthat::test_dir("variantkey/tests")'
