echo -e "\n\n*** BUILD ***\n\n"
mkdir -p "${PREFIX}/include"
cp -r ${RECIPE_DIR}/../../c/src/*.h "${PREFIX}/include"
