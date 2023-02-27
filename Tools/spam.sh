i=0
while [ $i != 100 ];
do
	response=$(curl  -H  'Content-Type: application/json' -F 'file=@/Users/klaarous/Desktop/webServer/cgi-bin/php-cgi' -X POST 127.0.0.1:3031)
	if [ "$?" != 0 ]; then
		echo Failed
	fi
	i=$((i+1))
done
rm -rf ./public/uploads/*