import { Tabs } from 'expo-router';
import React, { useState } from 'react';
import { Pressable, View, Text, StyleSheet, Alert } from 'react-native';

import { HapticTab } from '@/components/haptic-tab';
import { HeaderLeftBack } from '@/components/HeaderLeftBack';
import { HeaderLeftMenu } from '@/components/HeaderLeftMenu';
import { IconSymbol } from '@/components/ui/icon-symbol';
import { Colors } from '@/constants/theme';
import { useColorScheme } from '@/hooks/use-color-scheme';
import { useAuth } from '@/contexts/AuthContext';


export default function TabLayout() {
  const colorScheme = useColorScheme();
  const { user, signOut, isDev } = useAuth();
  const [showUserMenu, setShowUserMenu] = useState(false);

  const handleLogout = () => {
    Alert.alert(
      'Logout',
      'Are you sure you want to logout?',
      [
        { text: 'Cancel', style: 'cancel' },
        { 
          text: 'Logout', 
          style: 'destructive',
          onPress: async () => {
            await signOut();
            setShowUserMenu(false);
          }
        }
      ]
    );
  };

  // Header right component shared between tabs if desired
  const renderHeaderRight = () => (
    <View style={{ paddingRight: 16, position: 'relative' }}>
      <Pressable onPress={() => setShowUserMenu(!showUserMenu)}>
        <IconSymbol size={24} name="person.circle.fill" color={Colors[colorScheme ?? 'light'].icon} />
      </Pressable>
      
      {showUserMenu && (
        <View style={[styles.userMenu, { backgroundColor: colorScheme === 'dark' ? '#2a2a2a' : 'white' }]}>
          <Text style={[styles.userEmail, { color: Colors[colorScheme ?? 'light'].text }]}>
            {user?.email}
          </Text>
          <Pressable 
            style={styles.logoutButton}
            onPress={handleLogout}
          >
            <IconSymbol size={18} name="arrow.right.square" color="#ff3b30" />
            <Text style={styles.logoutText}>Logout</Text>
          </Pressable>
        </View>
      )}
    </View>
  );

  return (
      <Tabs
      screenOptions={{
        tabBarActiveTintColor: Colors[colorScheme ?? 'light'].tint,
        headerShown: true, // change to false if you do not want any header
        headerTintColor: Colors[colorScheme ?? 'light'].icon,
        tabBarButton: HapticTab,
      }}
    >
      <Tabs.Screen
        name="request"
        options={{
          title: 'Request',
          tabBarIcon: ({ color }) => <IconSymbol size={28} name="house.fill" color={color} />,
          headerRight: renderHeaderRight,
          headerLeft: () => <HeaderLeftMenu />,
        }}
      />
      <Tabs.Screen
        name="viewreports"
        options={{
          title: 'View Reports',
          tabBarIcon: ({ color }) => <IconSymbol size={28} name="paperplane.fill" color={color} />,
          headerRight: renderHeaderRight,
          headerLeft: () => <HeaderLeftMenu />,
        }}
      />

      
      <Tabs.Screen
        name="forms"
        options={{
          title: 'Forms',
          tabBarIcon: ({ color }) => <IconSymbol size={28} name="chevron.left.forwardslash.chevron.right" color={color} />,
          headerLeft: () => <HeaderLeftBack />,
          tabBarStyle: { display: 'none' },
          href: null, // Hide from tab bar but keep in navigation stack
          headerShown : false,
        }}
      />
      {isDev && (
        <Tabs.Screen
          name="admin"
          options={{
            title: 'Admin',
            tabBarIcon: ({ color }) => <IconSymbol size={28} name="person.circle.fill" color={color} />,
            headerRight: renderHeaderRight,
            headerLeft: () => <HeaderLeftMenu />,
            //href: '/admin',
          }}
        />
      )}
      
    </Tabs>
  );
}

const styles = StyleSheet.create({
  userMenu: {
    position: 'absolute',
    top: 35,
    right: 0,
    minWidth: 200,
    borderRadius: 8,
    padding: 12,
    shadowColor: '#000',
    shadowOffset: {
      width: 0,
      height: 2,
    },
    shadowOpacity: 0.25,
    shadowRadius: 3.84,
    elevation: 5,
    zIndex: 1000,
  },
  userEmail: {
    fontSize: 14,
    marginBottom: 12,
    paddingBottom: 12,
    borderBottomWidth: 1,
    borderBottomColor: '#e0e0e0',
  },
  logoutButton: {
    flexDirection: 'row',
    alignItems: 'center',
    gap: 8,
  },
  logoutText: {
    color: '#ff3b30',
    fontSize: 16,
    fontWeight: '500',
  },
});
