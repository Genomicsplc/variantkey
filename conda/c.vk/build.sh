echo -e "\n\n*** BUILD ***\n\n"
mkdir -p "${PREFIX}/bin"
cp "${RECIPE_DIR}/../../c/target/build/vk/vk" "${PREFIX}/bin/"
