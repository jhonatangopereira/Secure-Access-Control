import React, { useEffect, useState} from 'react';
import {SafeAreaView, Button, Alert,TouchableOpacity,NativeModules, NativeEventEmitter,Text,Dimensions,StyleSheet,FlatList,View} from 'react-native';
import BleManager from 'react-native-ble-manager';

const BleManagerModule = NativeModules.BleManager;
const BleManagerEmitter = new NativeEventEmitter(BleManagerModule);

const BluetoothComponent = (props) => {

  const { Token } = props;
  //Lista de dispositivos encontrados
  const [discoveredDevices, setDiscoveredDevices] = useState([]);
  const [discoveredDeviceIds, setDiscoveredDeviceIds] = useState(new Set());

  //conexao dispositivo
  const [connectedDevice, setConnectedDevice] = useState(null);

  const [fileText, setFileText] = useState(''); // Texto a ser enviado


  const connectToDevice = (deviceId) => {
    BleManager.connect(deviceId)
      .then(() => {
        console.log('Conectado ao dispositivo:', deviceId);
        setConnectedDevice(deviceId); // Salva o ID do dispositivo conectado
        
        })
      .catch(error => {
        console.error('Erro ao conectar ao dispositivo:', error);
      });
  };

 
  //ATIVIANDO O GERENCIAMENTO BLUETOOTH
  useEffect(() => {
    // start bluetooth manager
    BleManager.start({showAlert: false}).then(() => {
        console.log('BleManager initialized');
        
    });
  }, []);
  
  useEffect(() => {
    const discoverPeripheralListener = BleManagerEmitter.addListener(
      'BleManagerDiscoverPeripheral',
      (peripheral) => {

        if (!discoveredDeviceIds.has(peripheral.id)) {

          const updatedDevices = [...discoveredDevices, peripheral];
          setDiscoveredDevices(updatedDevices);
          setDiscoveredDeviceIds(new Set(discoveredDeviceIds.add(peripheral.id)));
        }
      }
    );
  
    return () => {
      discoverPeripheralListener.remove();
    };
  }, [discoveredDevices, discoveredDeviceIds]);

  //scanner bluetooth
  const [isScanning, setIsScanning] = useState(false);
  
  useEffect(() => {
      let stopListener = BleManagerEmitter.addListener(
      'BleManagerStopScan',
      () => {
        setIsScanning(false);
        console.log('Scan is stopped');

        },
      );
    }, []);
  
  const startScan = () => {
    //APAGA A FLAT LIST E OS IDS
    setDiscoveredDevices([]);
    setDiscoveredDeviceIds(new Set());

    if (!isScanning) {
        BleManager.scan([], 5, true)
          .then(() => {
            console.log('Scanning...');
            setIsScanning(true);
          })
          .catch(error => {
            console.error(error);
          });
        }
      };

    //Precisamos converter a string com o token para uma matriz dados para ser enviado
    function stringToBytes(str) {
        const bytes = [];
        for (let i = 0; i < str.length; i++) {
          bytes.push(str.charCodeAt(i));
        }
        return bytes;
      }
    //ENVIAR MENSAGEM
    const sendTextFile = () => {
        if (!connectedDevice) {
          console.error('Nenhum dispositivo conectado.');
          return;

        }
    
        //let textToSend = 'ligar'; // Substitua pelo texto que deseja enviar
        console.log(Token);
        const bytesToSend = stringToBytes(Token);
        console.log(bytesToSend);
        BleManager.writeWithoutResponse(
          connectedDevice,
          '00000180-0000-1000-8000-00805F9B34FB',
          '0000DEAD-0000-1000-8000-00805F9B34FB',
          bytesToSend
        )
          .then(() => {
            console.log('Arquivo de texto enviado com sucesso!');
          })
          .catch(error => {
            console.error('Erro ao enviar o arquivo de texto:', error);
          });
      };

      const disconnectDevice = () => {
        BleManager.disconnect(connectedDevice)
          .then(() => {
            console.log('Dispositivo desconectado.');
            setConnectedDevice(null); // Resetando o estado de conexão
            
          })
          .catch(error => {
            console.error('Erro ao desconectar:', error);
          });
      };

  return (
    <SafeAreaView style={styles.mainBody}>
          {!connectedDevice && (
          <TouchableOpacity
            activeOpacity={0.5}
            style={styles.buttonStyle}
            onPress={startScan}>
            <Text style={styles.buttonTextStyle}>
              {isScanning ? 'Scanning...' : 'Scan Bluetooth Devices'}
            </Text>
          </TouchableOpacity>
          )}

          {!connectedDevice && (
          <FlatList
            data={discoveredDevices}
            keyExtractor={(item) => item.id}
            renderItem={({ item }) => (
              <TouchableOpacity onPress={() => connectToDevice(item.id)}>
                <View style={styles.deviceContainer}>
                <Text style={styles.deviceItem}>
                  {item.name || 'NO NAME'}
                  </Text>
                  <Text style={styles.deviceAddress}>
                   {item.id || item.uuid || 'Endereço MAC não disponível' }
                  </Text>
                  <Text style={styles.deviceRSSI}>
                  {`RSSI: ${item.rssi || 'Não disponível'}`}
                  </Text>
                  </View>
                
              </TouchableOpacity>
          )}/>
          )}

      {connectedDevice && (
          <TouchableOpacity onPress={sendTextFile}
                          style = {styles.button}>
          <Text style={styles.texto}>Enviar Token</Text>
        </TouchableOpacity>
      )}

    {connectedDevice && (
      <TouchableOpacity onPress={disconnectDevice}
                        style={styles.buttonStyleRed}>
        <Text style={styles.buttonTextStyle}>Desconectar</Text>

      </TouchableOpacity>
    )}
      
    
      </SafeAreaView>
  );
};

export default BluetoothComponent;

const styles = StyleSheet.create({
  mainBody: {
    flex: 1,
    justifyContent: 'center',
    
  },
  texto: {
    position:'absolute',
    justifyContent: 'center',
    alignContent: 'center',
    color:'#ffffff',
    fontWeight:'700',
    fontSize:20,
    marginLeft:65
  },
  button: {
    backgroundColor: 'blue',
    justifyContent: 'center',
    alignContent: 'center',
    borderWidth: 3,
    borderRadius: (250 / 2),
    width: 250 ,
    height: 250 ,
  },
  buttonStyle: {
    backgroundColor: '#307ecc',
    borderWidth: 0,
    color: '#FFFFFF',
    borderColor: '#307ecc',
    height: 40,
    width:140,
    alignItems: 'center',
    borderRadius: 30,
    marginLeft: 35,
    marginRight: 35,
    marginTop: 15,
  },
  buttonStyleRed: {
    backgroundColor: '#ff0000',
    borderWidth: 0,
    color: '#FFFFFF',
    borderColor: '#307ecc',
    height: 40,
    alignItems: 'center',
    borderRadius: 30,
    marginLeft: 35,
    marginRight: 35,
    marginTop: 15,
  },
  buttonTextStyle: {
    color: '#FFFFFF',
    paddingVertical: 10,
    fontSize: 16,
  },
  deviceContainer: {
    marginTop:20,
    padding: 10,
    marginVertical: 5,
    marginHorizontal: 10,
    borderRadius: 10,
    backgroundColor: '#f0f0f0',
    shadowColor: '#000',
    shadowOffset: {
      width: 0,
      height: 2,
    },
    shadowOpacity: 0.25,
    shadowRadius: 3.84,
    elevation: 5,
  },
  
  deviceName: {
    fontSize: 18,
    fontWeight: 'bold',
    marginBottom: 5,
  },
  deviceAddress: {
    fontSize: 14,
    color: '#888',
  },
  deviceRSSI: {
    fontSize: 14,
    color: '#888',
  },
});