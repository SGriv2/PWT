echo "Информация о процессоре:"
echo "-----------------------"
echo "Модель процессора: $(lscpu | grep 'Model name' | awk -F: '{print $2}' | sed 's/^ //')"
echo "Микроархитектура: $(lscpu | grep 'Architecture' | awk -F: '{print $2}' | sed 's/^ //')"
echo "Число ядер: $(lscpu | grep 'Core(s) per socket' | awk -F: '{print $2}' | sed 's/^ //')"
echo "Включен ли HyperThreading: $(lscpu | grep 'Thread(s) per core' | awk -F: '{print $2}' | sed 's/^ //')"
echo "Размер кеша L1: $(lscpu | grep 'L1d cache' | awk -F: '{print $2}' | sed 's/^ //')"
echo "Размер кеша L2: $(lscpu | grep 'L2 cache' | awk -F: '{print $2}' | sed 's/^ //')"
echo "Размер кеша L3: $(lscpu | grep 'L3 cache' | awk -F: '{print $2}' | sed 's/^ //')"
echo

echo "Информация об оперативной памяти:"
echo "--------------------------------"
echo "Объем оперативной памяти: $(free -h | grep 'Mem:' | awk '{print $2}')"