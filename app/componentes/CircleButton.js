import React from 'react';
import {View, StyleSheet, TouchableOpacity, Text } from 'react-native';


export default class CircleButton extends React.Component {
    render(){
      let localStyles = styles(this.props)
      
      return (
        <View style={localStyles.container}>
          <TouchableOpacity
            activeOpacity={.8}
            style = {localStyles.button}
            onPress = {this.props.onPress}
          >
            <Text>React</Text>
            {this.props.children}
          </TouchableOpacity>
        </View>
      )
    }
  }

const styles = (props) => StyleSheet.create({
  container: {
    position: 'relative',
    zIndex: 0,
  },
  button: {
    backgroundColor: 'blue',
    justifyContent: 'center',
    alignContent: 'center',
    borderWidth: 3,
    borderRadius: (props.circleDiameter / 2),
    width: props.circleDiameter,
    height: props.circleDiameter,
  },
});
