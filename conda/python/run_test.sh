echo -e "\n\n*** TEST ***\n\n"
cd ${PROJECT_ROOT} && ${PYTHON} setup.py test

# generate source code documentation
DOCDIR="${PROJECT_ROOT}/target/doc"
mkdir -p ${DOCDIR}
rm -rf "${DOCDIR}/*"
cd ${DOCDIR}
pydoc -w variantkey
