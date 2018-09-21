echo -e "\n\n*** BUILD ***\n\n"
BINDIR="${PREFIX}/bin"
mkdir -p ${BINDIR}
cp "${RECIPE_DIR}/../../c/target/build/vk/vk" ${BINDIR}
DOCDIR="${PREFIX}/share/doc/vk/"
mkdir -p ${DOCDIR}
cp "${RECIPE_DIR}/../../README.md" ${DOCDIR}
cp "${RECIPE_DIR}/../../RELEASE" ${DOCDIR}
cp "${RECIPE_DIR}/../../VERSION" ${DOCDIR}
cp "${RECIPE_DIR}/../../LICENSE" ${DOCDIR}
