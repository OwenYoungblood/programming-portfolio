import React, { useEffect, useState } from 'react';
import { View, Text, StyleSheet, TouchableOpacity, useColorScheme, Image, Alert } from 'react-native';
import { ThemedView } from '@/components/themed-view';
import { ThemedText } from '@/components/themed-text';
import PrimaryButton from '@/components/ui/PrimaryButton';
import { useRouter } from 'expo-router';
import { supabase } from '../../../supabase';
import { Colors } from '@/constants/theme';
import { useFormData } from '@/app/context/requestFormContext';
import * as FileSystem from 'expo-file-system';
import { Buffer } from 'buffer';

export default function ReportRequest4Screen() {
  const [rating, setRating] = useState<number>(0);
  const [gridPath, setGridPath] = useState<string>('');
  const [readingPath, setReadingPath] = useState<string>('');
  const { data, updateStep } = useFormData();
  const router = useRouter();
  const colorScheme = useColorScheme();

  const handleStarPress = (selectedRating: number) => {
    setRating(selectedRating);
  };

const handleSubmit = async () => {
  console.log('Rating submitted:', rating);

  try {
    const gridPDF = data.request1?.completedGridImages;
    const readingPDF = data.request2?.completedReadingImages;

    if (!gridPDF || !readingPDF) {
      Alert.alert("Error", "Missing PDF files to upload.");
      return;
    }

    // Create unique filenames
    const timestamp = Date.now();
    const gridFileName = `uploads/${timestamp}_grid.pdf`;
    const readingFileName = `uploads/${timestamp}_reading.pdf`;

    console.log('gridPDF:', gridPDF);
    console.log('readingPDF:', readingPDF);

    // Use new FileSystem.File API
    const gridFile = new FileSystem.File(gridPDF);
    const readingFile = new FileSystem.File(readingPDF);

    // Use base64() method
    const gridBase64 = await gridFile.base64();
    const readingBase64 = await readingFile.base64();

    const gridBuffer = Buffer.from(gridBase64, 'base64');
    const readingBuffer = Buffer.from(readingBase64, 'base64');

    // Upload to Supabase Storage
    const { error: gridError } = await supabase.storage
      .from('completed_grid_images')
      .upload(gridFileName, gridBuffer, {
        contentType: 'application/pdf',
        upsert: true,
      });
    if (gridError) throw gridError;

    const { error: readingError } = await supabase.storage
      .from('completed_reading_images')
      .upload(readingFileName, readingBuffer, {
        contentType: 'application/pdf',
        upsert: true,
      });
    if (readingError) throw readingError;

    // Retrieve public URLs
    const { data: gridPublic } = supabase.storage
      .from('completed_grid_images')
      .getPublicUrl(gridFileName);

    const { data: readingPublic } = supabase.storage
      .from('completed_reading_images')
      .getPublicUrl(readingFileName);

    // Insert into table
    const { error: insertError } = await supabase.from('report_requests').insert([
      {
        street: data.request1?.street,
        city: data.request1?.city,
        state: data.request1?.stateVal,
        zip: data.request1?.zip,
        issue: data.request1?.issue,
        completedGridImages: gridPublic.publicUrl,
        contact: data.request1?.contact,
        date_requested_by: data.request1?.dateRequestedBy?.toISOString(),
        rush_requested: data.request1?.rushSelection,
        redry_bid: data.request1?.redrySelection,
        multipleLayers: data.request2?.multipleLayers,
        membraneType: data.request2?.membraneType,
        insulationTypes: data.request2?.insulationTypes,
        mexsSetting: data.request2?.mexsSetting,
        relFloor: data.request2?.relFloor,
        relCalibration: data.request2?.relCalibration,
        pinPrickCalibration: data.request2?.pinPrickCalibration,
        completedReadingImages: readingPublic.publicUrl,
        expedited: data.request3?.expedited || false,
        rating,
        created_at: new Date().toISOString(),
      },
    ]);

    if (insertError) throw insertError;

    Alert.alert('✅ Success', 'Your MRI Report has been submitted successfully!');
    router.push('../request');
  } catch (error) {
    console.error(error);
    Alert.alert('❌ Error', 'There was a problem submitting your report. Please try again.');
  }
};



  const handleBack = () => {
    router.back();
  };

  return (
    <View style={styles.container}>
      <View style={[styles.card, { marginTop: 112 }, { marginBottom: 177 }]}>
        <View style={[styles.header, { marginTop: -18 }]}>
          <View style={styles.headerRow}>
            <View style={styles.textContainer}>
              <Text style={[styles.title, { color: Colors[colorScheme ?? 'light'].text }]}>
                GET YOUR REPORT!
              </Text>
              <Text style={[styles.thankYouMessage, { color: Colors[colorScheme ?? 'light'].icon }]}>
                Thanks for being a Roof MRI Certified{'\n'}Contractor.
              </Text>
            </View>
            
            {/* Right side - RoofMRI Logo */}
            <View style={styles.logoContainer}>
              <Image
                source={require('@/assets/images/roofMRI.png')}
                style={styles.logoImage}
                resizeMode="contain"
                onError={(error) => console.log('Image loading error:', error)}
                onLoad={() => console.log('Image loaded successfully')}
              />
            </View>
          </View>
        </View>

        {/* Rating Section */}
        <View style={styles.ratingSection}>
          <Text style={[styles.ratingQuestion, { color: Colors[colorScheme ?? 'light'].text }]}>
            How would you rate this scan{'\n'}experience?
          </Text>
          
          <View style={styles.starsContainer}>
            {[1, 2, 3, 4, 5].map((star) => (
              <TouchableOpacity
                key={star}
                onPress={() => handleStarPress(star)}
                style={styles.starButton}
              >
                <Text style={[
                  styles.star,
                  { color: star <= rating ? '#FFC107' : '#E0E0E0' }
                ]}>
                  ★
                </Text>
              </TouchableOpacity>
            ))}
          </View>
        </View>

        {/* Action Buttons */}
        <View style={styles.buttonContainer}>
          <TouchableOpacity 
            onPress={handleBack}
            style={[styles.backButton, { backgroundColor: '#9E9E9E' }]}
          >
            <Text style={styles.backButtonText}>Back</Text>
          </TouchableOpacity>
          
          <TouchableOpacity 
            onPress={handleSubmit}
            style={[styles.submitButton, { backgroundColor: '#4CAF50' }]}
          >
            <Text style={styles.submitButtonText}>Submit</Text>
          </TouchableOpacity>
        </View>
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'flex-start',
    alignItems: 'center',
    padding: 20,
    backgroundColor: '#ecf6fdff', // 🩵 background behind the card ecf6fdff
  },
  card: {
    backgroundColor: '#ffffff', // 🤍 make the card itself white for contrast
    width: '100%',
    maxWidth: 500,
    borderRadius: 20,
    padding: 40,
    shadowColor: '#000',
    shadowOffset: {
      width: 0,
      height: 4,
    },
    shadowOpacity: 0.1,
    shadowRadius: 8,
    elevation: 5,
    alignItems: 'center',
    justifyContent: 'flex-start',
  },
  header: {
    alignItems: 'flex-start',
    width: '100%',
    marginBottom: 30,
  },
  headerRow: {
    flexDirection: 'row',
    alignItems: 'flex-start',
    justifyContent: 'space-between',
    width: '100%',
    paddingHorizontal: 10,
  },
  textContainer: {
    flex: 6,
    paddingRight: 15,
  },
  title: {
    fontSize: 24,
    fontWeight: 'bold',
    textAlign: 'left',
    marginBottom: 8,
  },
  logoContainer: {
    alignItems: 'center',
    justifyContent: 'center',
    height: 90, 
    borderRadius: 8,
    marginLeft: 10,
    width: 100,
    flexShrink: 0,
  },
  logoImage: {
    width: 95,
    height: 75,
  },
  thankYouMessage: {
    fontSize: 16,
    textAlign: 'left',
    lineHeight: 22,
  },
  ratingSection: {
    alignItems: 'center',
    width: '100%',
    marginBottom: 30,
  },
  ratingQuestion: {
    fontSize: 18,
    textAlign: 'center',
    marginBottom: 20,
    lineHeight: 26,
  },
  starsContainer: {
    flexDirection: 'row',
    justifyContent: 'center',
    gap: 12,
  },
  starButton: {
    padding: 8,
  },
  star: {
    fontSize: 36,
    fontWeight: 'bold',
  },
  buttonContainer: {
    flexDirection: 'row',
    width: '100%',
    gap: 20,
    justifyContent: 'center',
    paddingHorizontal: 20,
  },
  backButton: {
    flex: 1,
    paddingVertical: 18,
    borderRadius: 10,
    alignItems: 'center',
    justifyContent: 'center',
  },
  backButtonText: {
    color: 'white',
    fontSize: 18,
    fontWeight: '600',
  },
  submitButton: {
    flex: 1,
    paddingVertical: 18,
    borderRadius: 10,
    alignItems: 'center',
    justifyContent: 'center',
  },
  submitButtonText: {
    color: 'white',
    fontSize: 18,
    fontWeight: '600',
  },
});