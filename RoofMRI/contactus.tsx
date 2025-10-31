import React from 'react';
import { Platform, StyleSheet, View, Pressable, Alert } from 'react-native';
import { Ionicons } from '@expo/vector-icons';
import * as Linking from 'expo-linking';

import { ThemedText } from '@/components/themed-text';
import { ThemedView } from '@/components/themed-view';
import { useColorScheme } from '@/hooks/use-color-scheme';

import { ScrollView } from 'react-native';
// Only import WebView on non-web
const WebView = Platform.OS === 'web' ? null : require('react-native-webview').WebView;

const calendlyUrl =
  'https://calendly.com/lincolnbethinkin/30min?embed_domain=yourdomain.com&embed_type=Inline';

export default function ContactScreen() {
  const colorScheme = useColorScheme();

  const handleEmail = async () => {
    const email = 'adam@re-dry.com';
    const subject = 'Roof question';
    const body = 'This is a test email.';
    const url = `mailto:${email}?subject=${encodeURIComponent(subject)}&body=${encodeURIComponent(body)}`;
    const canOpen = await Linking.canOpenURL(url);
    if (canOpen) {
      await Linking.openURL(url);
    } else {
      Alert.alert('No email client available');
    }
  };

  const handleCall = async () => {
    const phoneNumber = 'tel:+18657713848';
    const canOpen = await Linking.canOpenURL(phoneNumber);
    if (canOpen) {
      await Linking.openURL(phoneNumber);
    } else {
      Alert.alert('No phone app available');
    }
  };

  return (
  <ThemedView style={styles.container}>
    <ScrollView contentContainerStyle={styles.scrollContent} keyboardShouldPersistTaps="handled">
      <ThemedView style={styles.contentContainer}>
        {/* Titles */}
        <ThemedView style={styles.titleWrapper}>
          <ThemedText type="title" style={styles.mainTitle}>Still Need Help?</ThemedText>
          <ThemedText type="default" style={styles.subtitle}>Contact us about any questions you have!</ThemedText>
        </ThemedView>

        {/* Contact info */}
        <ThemedView style={[
          styles.contactInfo,
          { backgroundColor: colorScheme === 'dark' ? 'rgba(255, 255, 255, 0.05)' : 'rgba(255, 255, 255, 0.1)' }
        ]}>
          <ThemedText type='defaultSemiBold' style={styles.contactLabel}>Contact Information:</ThemedText>
          <ThemedText style={styles.contactDetail}>📧 adam@re-dry.com</ThemedText>
          <ThemedText style={styles.contactDetail}>📞 (865) 771-3848</ThemedText>
        </ThemedView>

        {/* Buttons */}
        <ThemedView style={styles.buttonContainer}>
          <Pressable onPress={handleEmail} style={[styles.button, styles.emailButton]}>
            <View style={styles.buttonContent}>
              <Ionicons name="mail" size={24} color="white" />
              <ThemedText type="link" style={styles.buttonText}>Email Question</ThemedText>
            </View>
          </Pressable>
          <Pressable onPress={handleCall} style={[styles.button, styles.callButton]}>
            <View style={styles.buttonContent}>
              <Ionicons name="call" size={24} color="white" />
              <ThemedText type="link" style={styles.buttonText}>Call For Help</ThemedText>
            </View>
          </Pressable>
        </ThemedView>

        {/* Calendly Inline Embed */}
        <View style={styles.calendlyWrapper}>
          {Platform.OS === 'web' ? (
            <div style={{
              width: '100%',
              height: '100%',
              borderRadius: '14px',
              overflow: 'hidden',
              background: 'white',
            }}>
              <iframe
                src={calendlyUrl}
                width="100%"
                height="100%"
                frameBorder="0"
                style={{ border: 'none', overflow: 'hidden' }}
                title="Schedule with Calendly"
              />
            </div>
          ) : (
            <WebView
              source={{ uri: calendlyUrl }}
              style={styles.webview}
              javaScriptEnabled
              domStorageEnabled
              startInLoadingState
            />
          )}
        </View>

      </ThemedView>
    </ScrollView>
  </ThemedView>
);
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  contentContainer: {
    flex: 1,
    padding: 20,
    paddingTop: 60,
  },
  titleWrapper: {
    marginTop: 0,
    marginBottom: 40,
    alignItems: 'center',
    zIndex: 1,
  },
  mainTitle: {
    fontSize: 28,
    fontWeight: 'bold',
    textAlign: 'center',
    marginBottom: 12,
  },
  subtitle: {
    fontSize: 16,
    textAlign: 'center',
    opacity: 0.8,
    paddingHorizontal: 20,
    lineHeight: 22,
  },
  contactInfo: {
    borderRadius: 12,
    padding: 20,
    marginBottom: 30,
    alignItems: 'center',
  },
  contactLabel: {
    fontSize: 18,
    marginBottom: 12,
    textAlign: 'center',
  },
  contactDetail: {
    fontSize: 16,
    marginBottom: 8,
    textAlign: 'center',
    opacity: 0.9,
  },
  buttonContainer: {
    gap: 16,
    paddingHorizontal: 10,
    flexDirection: 'row',
    justifyContent: 'center',
  },
  button: {
    paddingVertical: 16,
    paddingHorizontal: 24,
    borderRadius: 12,
    elevation: 3,
    shadowColor: '#000',
    shadowOffset: {
      width: 0,
      height: 2,
    },
    shadowOpacity: 0.25,
    shadowRadius: 3.84,
  },
  scrollContent: {
    flexGrow: 1,
    paddingBottom: 20,
    backgroundColor: 'transparent',
  },
  callButton: {
    backgroundColor: "#28a745",
  },
  emailButton: {
    backgroundColor: "#4986c0ff",
  },
  buttonContent: {
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'center',
    gap: 0,
  },
  buttonText: {
    color: 'white',
    fontSize: 18,
    fontWeight: '600',
  },
  calendlyWrapper: {
  height: 400,  // smaller height for the calendar box
  width: '100%',
  maxWidth: 700,
  alignSelf: 'center',
  marginTop: 20,
  marginBottom: 40,
  borderRadius: 14,
  overflow: 'hidden',
},
webview: {
  flex: 1,
  width: '100%',
  height: '100%',
  borderRadius: 14,
  overflow: 'hidden',
}
});
