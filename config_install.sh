./configure --prefix=/opt/nginx --add-module=/home/vagrant/shared/echo-nginx-module --add-module=/home/vagrant/shared/nginx_devinsampa_output_filter

make
sudo make install
sudo rm /usr/local/bin/nginx
sudo ln -s /opt/nginx/sbin/nginx /usr/local/bin/nginx

