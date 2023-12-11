import React, { useEffect, useState } from "react"
import {SafeAreaView,Button, Text, StyleSheet, Alert, View,Platform, PermissionsAndroid} from 'react-native'
import SplashScreen from 'react-native-splash-screen'
import AsyncStorage from "@react-native-async-storage/async-storage";
import DeviceInfo from 'react-native-device-info';
import BleManager from 'react-native-ble-manager';
// import BluetoothStateManager from 'react-native-bluetooth-state-manager';

import CircleButton from './componentes/CircleButton'
import BluetoothComponent from './componentes/BluetoothComponent'

export default () => {

    useEffect(() => {
      SplashScreen.hide();
    },[]);
    //CRIANDO E SALVANDO O TOLKEN
    const [uniqueId, setUniqueId] = useState("");

    useEffect(() => {
        async function fetchUniqueId(){
            try{
                // Verifica se o ID único já foi salvo no AsyncStorage
                const storedUniqueId = await AsyncStorage.getItem("uniqueId");
    
                if (storedUniqueId) {
                    // Se existir, define o ID único armazenado no estado
                    setUniqueId(storedUniqueId);
            }else {
                // Se não existir, obtém o ID único usando DeviceInfo
                let uniqueIdd = await DeviceInfo.getUniqueId();
                console.log("teste",uniqueIdd);
                setUniqueId(uniqueIdd);
                // salva no asyncStorage pro futuro
                await AsyncStorage.setItem("uniqueId", uniqueIdd);
            }
            }catch (error) {
                console.error("Erro ao obter ID:", error);
            }
        }
        fetchUniqueId();
        }, []);

    //LIGANDO  BLUETOOTH
    useEffect(() => {
        // turn on bluetooth if it is not on
        BleManager.enableBluetooth().then(() => {
          console.log('Bluetooth is turned on!');
        });
    
          if (Platform.OS === 'android' && Platform.Version >= 23) {
          PermissionsAndroid.check(
            PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
          ).then(result => {
            if (result) {
              console.log('Permission is OK');
            } else {
              PermissionsAndroid.request(
                PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
              ).then(result => {
                if (result) {
                  console.log('User accept');
                } else {
                  console.log('User refuse');
                }
              });
            }
          });
        }
    
      }, []);

      

    return (
        
        <SafeAreaView style={style.App}>
            <BluetoothComponent Token={uniqueId}/>    
        </SafeAreaView>
        
        )
    }


const style = StyleSheet.create({
    App: {
    backgroundColor:'#fff',
    flexGrow: 1,
    justifyContent: "center",
    alignItems: "center",
    padding: 20
        }
    })