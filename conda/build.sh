echo -e "\n\n*** BUILD ***\n\n"
cd python && LDSHARED="${CC} -shared" ${PYTHON} setup.py install --single-version-externally-managed --record=record.txt
