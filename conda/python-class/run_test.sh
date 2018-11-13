echo -e "\n\n*** TEST ***\n\n"
export COVERAGE_PROCESS_START="${PROJECT_ROOT}/.coveragerc"
echo "import coverage; coverage.process_startup()" > "${SP_DIR}/subcov.pth"
COVDIR="${PROJECT_ROOT}/target/coverage"
mkdir -p ${COVDIR}
rm -rf "${COVDIR}/*"
cd ${PROJECT_ROOT} \
&& pycodestyle --max-line-length=120 pyvariantkey \
&& ${PYTHON} setup.py test \
&& cp .coverage ${COVDIR}/

# generate source code documentation
DOCDIR="${PROJECT_ROOT}/target/doc"
mkdir -p ${DOCDIR}
rm -rf "${DOCDIR}/*"
cd ${DOCDIR}
pydoc -w pyvariantkey.variantkey
