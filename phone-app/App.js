import React from 'react';
import { StyleSheet, Text, View, Button, Alert, SafeAreaView} from 'react-native';

const Separator = () => (
  <View style={styles.separator} />
);

export default function App() {
  return (
    
  <SafeAreaView style={styles.container}>
    <View>
      <Text style={styles.title}>
        For new users
      </Text>
      <Button
        title="Check in"
        color="#87D5D0"
        onPress={() => Alert.alert('For Checking in')}
      />
    </View>
    <Separator />
    <View>
      <Text style={styles.title}>
        Enable Wifi on Tiny Circuit
      </Text>
      <Button
        title="Connect"
        color="#87D5D0"
        onPress={() => Alert.alert('For connecting wifi')}
      />
    </View>
    <Separator />
    
  </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#fff',
    alignItems: 'center',
    justifyContent: 'center',
  },
  separator: {
    marginVertical: 8,
    borderBottomColor: '#000000',
    borderBottomWidth: StyleSheet.hairlineWidth,
  },
});
