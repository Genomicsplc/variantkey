echo -e "\n\n*** BUILD ***\n\n"
INCLUDEDIR="${PREFIX}/include/variantkey"
mkdir -p ${INCLUDEDIR}
find "${RECIPE_DIR}/../../c/src/variantkey/" -type f -name '*.h' -exec cp {} ${INCLUDEDIR} \;
DOCDIR="${PREFIX}/share/doc/variantkey-src/"
mkdir -p ${DOCDIR}
cp "${RECIPE_DIR}/../../README.md" ${DOCDIR}
cp "${RECIPE_DIR}/../../RELEASE" ${DOCDIR}
cp "${RECIPE_DIR}/../../VERSION" ${DOCDIR}
cp "${RECIPE_DIR}/../../LICENSE" ${DOCDIR}
