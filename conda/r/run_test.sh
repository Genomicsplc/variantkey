echo -e "\n\n*** TEST ***\n\n"
cd ${PROJECT_ROOT}
$R -e 'library(testthat);testthat::test_dir("variantkey/tests")'

# generate source code documentation
cd ${PROJECT_ROOT}/variantkey
$R -e 'pkgdown::build_site();'
