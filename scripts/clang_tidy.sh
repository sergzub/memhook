export ROOT_DIR=`/bin/pwd`;
grep "\"file\":" compile_commands.json | awk '{ print $2; }' | sed 's/\"//g' | while read FILE; do
  (cd $(dirname $FILE); clang-tidy-3.9 -p $ROOT_DIR `basename $FILE`);
done
