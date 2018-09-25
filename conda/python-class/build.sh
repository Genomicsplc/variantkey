echo -e "\n\n*** BUILD ***\n\n"
cd python-class && ${PYTHON} setup.py install --single-version-externally-managed --record=record.txt
