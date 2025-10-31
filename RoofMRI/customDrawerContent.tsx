import { IconSymbol } from '@/components/ui/icon-symbol'; // Make sure this import is correct
import { Colors } from '@/constants/theme';
import { useColorScheme } from '@/hooks/use-color-scheme';
import { DrawerContentScrollView, DrawerItem, DrawerItemList } from "@react-navigation/drawer";
import { useRouter } from "expo-router";
import { Image, View, Text } from "react-native";
import React, { useEffect, useState } from "react";
import { useSafeAreaInsets } from 'react-native-safe-area-context';
import { getCurrentUserProfile } from '@/services/userProfile';
import { useAuth } from '@/contexts/AuthContext';

export default function CustomDrawerContent(props: any) {
  const router = useRouter();
  const colorScheme = useColorScheme();
  const {top, bottom} = useSafeAreaInsets();
  const { user } = useAuth();
  const [isDev, setIsDev] = useState(false);

  useEffect(() => {
    let mounted = true;
    (async () => {
      const { profile } = await getCurrentUserProfile();
      if (!mounted) return;
      setIsDev(!!profile?.dev);
    })();
    return () => { mounted = false; };
  }, [user?.id]);

  return (
    <View style={{ flex: 1 }}>
      <DrawerContentScrollView
        {...props}
        scrollEnabled={false}
        contentContainerStyle={{backgroundColor: Colors[colorScheme ?? 'light'].tint}}>
        <View style={{padding: 20}}>
          <Image
            source={require('@/assets/images/roofMRI.png')}
            style={{ width: 100, height: 100, alignSelf: 'center' }} 
          />
        </View>
        <View style={{
          flex: 1, 
          backgroundColor: Colors[colorScheme ?? 'light'].background, 
          minHeight: '100%',
          marginHorizontal: -20,
          paddingHorizontal: 20,
          marginLeft: -16,
          marginRight: -16
        }}>
          <DrawerItem
            icon={({ color, size }) => (
              <IconSymbol
                name="house.fill"
                size={size}
                color={color} />
            )}
            label="Home"
            labelStyle={{ color: Colors[colorScheme ?? 'light'].text }}
            onPress={() => router.push('/(tabs)/request')} />
          <DrawerItemList {...props} />
          {isDev && (
            <DrawerItem
              icon={({ color, size }) => (
                <IconSymbol name="gear" size={size} color={color} />
              )}
              label="Admin"
              labelStyle={{ color: Colors[colorScheme ?? 'light'].text }}
              onPress={() => router.push('/admin')}
            />
          )}
        </View>
      </DrawerContentScrollView>
      <View
        style={{
          borderTopColor: Colors[colorScheme ?? 'light'].tint,
          borderTopWidth: 1,
          padding: 20,
          paddingBottom: 20 + bottom,
          backgroundColor: Colors[colorScheme ?? 'light'].background
        }}>
        <Text style={{ color: Colors[colorScheme ?? 'light'].text }}>Footer</Text>
      </View>
    </View>
  );
}
