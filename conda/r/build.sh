echo -e "\n\n*** BUILD ***\n\n"
cd r && make build
cp src/*.o src/*.so src/*.R -t "${PREFIX}/"
