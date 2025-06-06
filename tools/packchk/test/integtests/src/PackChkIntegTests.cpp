/*
 * Copyright (c) 2020-2025 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "PackChkIntegTestEnv.h"

#include "PackChk.h"
#include "ErrLog.h"

#include <fstream>

using namespace std;

class PackChkIntegTests : public ::testing::Test {
public:
  void SetUp()    override;
  void TearDown() override;

  string GetPackXsd();
  void CheckCopyPackXsd();
  void DeletePackXsd();
};

void PackChkIntegTests::SetUp() {
  CheckCopyPackXsd();
}

void PackChkIntegTests::TearDown() {
  ErrLog::Get()->Destroy();
}

string PackChkIntegTests::GetPackXsd() {
  const string schemaDestDir = string(PROJMGRUNITTESTS_BIN_PATH) + "/../etc";
  const string schemaFileName = schemaDestDir + "/PACK.xsd";

  return schemaFileName;
}

void PackChkIntegTests::CheckCopyPackXsd() {
  error_code ec;

  const string schemaDestDir = string(PROJMGRUNITTESTS_BIN_PATH) + "/../etc";
  const string schemaFileName = GetPackXsd();

  if (RteFsUtils::Exists(schemaFileName)) {
    return;
  }

  // Copy Pack.xsd
  const string packXsd = string(PACKXSD_FOLDER) + "/PACK.xsd";
  if (RteFsUtils::Exists(schemaDestDir)) {
    RteFsUtils::RemoveDir(schemaDestDir);
  }
  RteFsUtils::CreateDirectories(schemaDestDir);
  fs::copy(fs::path(packXsd), fs::path(schemaFileName), ec);
}

void PackChkIntegTests::DeletePackXsd() {
  const string schemaFileName = GetPackXsd();

  if (!RteFsUtils::Exists(schemaFileName)) {
    return;
  }

  // Delete Pack.xsd
  RteFsUtils::RemoveFile(schemaFileName);
  ASSERT_FALSE(RteFsUtils::Exists(schemaFileName));
}


 // Validate packchk when no .pdsc file found
TEST_F(PackChkIntegTests, FileNotAVailable) {
  const char* argv[2];

  argv[0] = (char*)"";
  argv[1] = (char*)"UNKNOWN.FILE.pdsc";

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(2, argv, nullptr));
}

TEST_F(PackChkIntegTests, FileNotAVailable2) {
  const char* argv[2];

  argv[0] = (char*)"";
  argv[1] = (char*)"UNKNOWN.FILE.pdsc";

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(2, argv, nullptr));
}


TEST_F(PackChkIntegTests, VersionOption) {
  const char* argv[2];

  argv[0] = (char*)"";
  argv[1] = (char*)"-V";

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(2, argv, nullptr));

  argv[1] = (char*)"--version";
  EXPECT_EQ(0, packChk.Check(2, argv, nullptr));
}

// Validate PackChk with invalid arguments
TEST_F(PackChkIntegTests, InvalidArguments) {
  const char *argv[3];

  const string& pdscFile = PackChkIntegTestEnv::globaltestdata_dir +
    "/packs/ARM/RteTest/0.1.0/ARM.RteTest.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*) pdscFile.c_str();
  argv[2] = (char*) "--invalid";

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(3, argv, nullptr));
}

// Validate software pack
TEST_F(PackChkIntegTests, CheckValidPack) {
  const char* argv[2];

  const string& pdscFile = PackChkIntegTestEnv::globaltestdata_dir +
    "/packs/ARM/RteTest/0.1.0/ARM.RteTest.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(2, argv, nullptr));
}

// Validate invalid software pack
TEST_F(PackChkIntegTests, CheckInvalidPack) {
  const char* argv[3];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/InvalidPack/TestVendor.TestInvalidPack.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(3, argv, nullptr));
}

// Validate software pack with component dependencies
TEST_F(PackChkIntegTests, CheckComponentDependency) {
  const char* argv[9];

  const string& pdscFile = PackChkIntegTestEnv::globaltestdata_dir +
    "/packs/ARM/RteTest/0.1.0/ARM.RteTest.pdsc";
  const string& refFile = PackChkIntegTestEnv::globaltestdata_dir +
    "/packs/ARM/RteTest_DFP/0.1.1/ARM.RteTest_DFP.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));
  ASSERT_TRUE(RteFsUtils::Exists(refFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"-i";
  argv[3] = (char*)refFile.c_str();
  argv[4] = (char*)"-x";
  argv[5] = (char*)"M334";
  argv[6] = (char*)"M324";
  argv[7] = (char*)"M362";
  argv[8] = (char*)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(9, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M382", 0)) != string::npos) {
      FAIL() << "error: contains warning M382";
    }
  }
}

// Check generation of pack file name
TEST_F(PackChkIntegTests, WritePackFileName) {
  const char* argv[4];
  ifstream file;

  const string& pdscFile = PackChkIntegTestEnv::globaltestdata_dir +
    "/packs/ARM/RteTest/0.1.0/ARM.RteTest.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  string line, outFile = PackChkIntegTestEnv::testoutput_dir + "/packname.txt";
  if (RteFsUtils::Exists(outFile)) {
    RteFsUtils::RemoveFile(outFile);
  }

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"-n";
  argv[3] = (char*)outFile.c_str();

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(4, argv, nullptr));
  EXPECT_TRUE(RteFsUtils::Exists(outFile));

  // Check output file content
  file.open(outFile);
  ASSERT_EQ(true, file.is_open()) << "Failed to open " << outFile;

  getline(file, line);
  EXPECT_EQ(line, "ARM.RteTest.0.1.0.pack");

  file.close();
}

// Verify that the specified URL matches the <url> element in the *.pdsc file
TEST_F(PackChkIntegTests, CheckPackServerURL) {
  const char* argv[4];

  const string& pdscFile = PackChkIntegTestEnv::globaltestdata_dir +
    "/packs/ARM/RteTest/0.1.0/ARM.RteTest.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"-u";
  argv[3] = (char*)"www.keil.com/pack/";

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(4, argv, nullptr));
}

// Suppress all listed validation messages
TEST_F(PackChkIntegTests, SuppressValidationMsgs) {
  const char* argv[6];

  const string& pdscFile = PackChkIntegTestEnv::globaltestdata_dir +
    "/packs/ARM/RteTest/0.1.0/ARM.RteTest.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"-x";
  argv[3] = (char*)"M382";
  argv[4] = (char*)"-x";
  argv[5] = (char*)"M324";

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(6, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M382")) != string::npos) {

      FAIL() << "error: contains warning M382";
    }

    if ((s = msg.find("M324")) != string::npos) {
      FAIL() << "error: contains warning M324";
    }
  }
}

TEST_F(PackChkIntegTests, AddRefPacks) {
  const char* argv[11];

  string outDir = PackChkIntegTestEnv::testoutput_dir + "/Packs";
  ASSERT_TRUE(RteFsUtils::CreateDirectories(outDir));

  string refNameTest = "TestPack";
  string refName1 = "RefPack1";
  string refName2 = "RefPack2";
  string refName3 = "RefPack3";
  string refName4 = "RefPack4";

  string refPackTest = "/" + refNameTest;
  string refPack1 = "/" + refName1;
  string refPack2 = "/" + refName2;
  string refPack3 = "/" + refName3;
  string refPack4 = "/" + refName4;
  ASSERT_TRUE(RteFsUtils::CreateDirectories(outDir + refPackTest));
  ASSERT_TRUE(RteFsUtils::CreateDirectories(outDir + refPack1));
  ASSERT_TRUE(RteFsUtils::CreateDirectories(outDir + refPack2));
  ASSERT_TRUE(RteFsUtils::CreateDirectories(outDir + refPack3));
  ASSERT_TRUE(RteFsUtils::CreateDirectories(outDir + refPack4));

  refPackTest += "/" + refNameTest + ".pdsc";
  refPack1 += "/" + refName1 + ".pdsc";
  refPack2 += "/" + refName2 + ".pdsc";
  refPack3 += "/" + refName3 + ".pdsc";
  refPack4 += "/" + refName4 + ".pdsc";
  const string contentBegin = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"\
                              "<package schemaVersion=\"1.3\" xmlns:xs=\"http://www.w3.org/2001/XMLSchema-instance\" xs:noNamespaceSchemaLocation=\"PACK.xsd\">\n"\
                              "  <name>";
  const string contentEnd =   "  </name>\n"\
                              "</package>\n";

  refPackTest = outDir + refPackTest;
  refPack1 = outDir + refPack1;
  refPack2 = outDir + refPack2;
  refPack3 = outDir + refPack3;
  refPack4 = outDir + refPack4;

  ASSERT_TRUE(RteFsUtils::CreateTextFile(refPackTest, contentBegin + refNameTest + contentEnd));
  ASSERT_TRUE(RteFsUtils::CreateTextFile(refPack1, contentBegin + refName1 + contentEnd));
  ASSERT_TRUE(RteFsUtils::CreateTextFile(refPack2, contentBegin + refName2 + contentEnd));
  ASSERT_TRUE(RteFsUtils::CreateTextFile(refPack3, contentBegin + refName3 + contentEnd));
  ASSERT_TRUE(RteFsUtils::CreateTextFile(refPack4, contentBegin + refName4 + contentEnd));

  ASSERT_TRUE(RteFsUtils::Exists(refPackTest));
  ASSERT_TRUE(RteFsUtils::Exists(refPack1));
  ASSERT_TRUE(RteFsUtils::Exists(refPack2));
  ASSERT_TRUE(RteFsUtils::Exists(refPack3));
  ASSERT_TRUE(RteFsUtils::Exists(refPack4));

  argv[0] = (char*)"";
  argv[1] = (char*)refPackTest.c_str();
  argv[2] = (char*)"-i";
  argv[3] = (char*)refPack1.c_str();
  argv[4] = (char*)"-i";
  argv[5] = (char*)refPack2.c_str();
  argv[6] = (char*)"-i";
  argv[7] = (char*)refPack3.c_str();
  argv[8] = (char*)"-i";
  argv[9] = (char*)refPack4.c_str();
  argv[10] = (char*)"--disable-validation";

  PackChk packChk;
  packChk.Check(11, argv, nullptr);

  const RteGlobalModel& model = packChk.GetModel();
  const RtePackageMap& packs = model.GetPackages();

  for(auto& [name, pack] : packs) {
    if(name != refNameTest && name != refName1 && name != refName2 && name != refName3 && name != refName4) {
      FAIL() << "RefPack was not added";
    }
  }

}

// Validate software pack with directory starting by .
TEST_F(PackChkIntegTests, CheckPackWithDot) {
  const char* argv[2];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/TestPackDot/TestVendor.TestPackDot.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(2, argv, nullptr));
}


// Validate software pack with SemVer semantic versioning
TEST_F(PackChkIntegTests, CheckSemVer) {
  const char* argv[3];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/SemVerTest/Arm.SemVerTest_DFP.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(3, argv, nullptr));

  struct {
    int M329 = 0;
    int M394 = 0;
    int M393 = 0;
    int M396 = 0;
  } cnt;

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  for (const string& msg : errMsgs) {
    size_t s;

    if ((s = msg.find("M329", 0)) != string::npos) {
      cnt.M329++;
    }
    if ((s = msg.find("M393", 0)) != string::npos) {
      cnt.M393++;
    }
    if ((s = msg.find("M394", 0)) != string::npos) {
      cnt.M394++;
    }
    if ((s = msg.find("M396", 0)) != string::npos) {
      cnt.M396++;
    }
  }

  if(cnt.M329 != 2 || cnt.M393 != 3 || cnt.M394 != 4 || cnt.M396 != 3) {
    FAIL() << "Occurrences of M329, M393, M394, M396 are wrong.";
  }
}


// Validate license path
TEST_F(PackChkIntegTests, CheckPackLicense) {
  const char* argv[3];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/TestLicense/TestVendor.TestPackLicense.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(3, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M300_foundCnt = 0;
  int M327_foundCnt = 0;
  int M367_foundCnt = 0;
  int M601_foundCnt = 0;
  int M602_foundCnt = 0;
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M300")) != string::npos) {
      M300_foundCnt++;
    }
    if ((s = msg.find("M327")) != string::npos) {
      M327_foundCnt++;
    }
    if ((s = msg.find("M367")) != string::npos) {
      M367_foundCnt++;
    }
    if ((s = msg.find("M601")) != string::npos) {
      M601_foundCnt++;
    }
    if ((s = msg.find("M602")) != string::npos) {
      M602_foundCnt++;
    }
  }

  if (M300_foundCnt != 1 || M327_foundCnt != 1 || M367_foundCnt != 1 || M601_foundCnt != 1 || M602_foundCnt != 1) {
    FAIL() << "error: missing license check warnings";
  }
}

// Validate CPU feature SON
TEST_F(PackChkIntegTests, CheckFeatureSON) {
  const char* argv[3];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/TestSON/TestVendor.TestSON.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char *)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(3, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  bool bFound = false;
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M371")) != string::npos) {
      bFound = true;
      break;
    }
  }

  if (!bFound) {
    FAIL() << "error: missing error M371";
  }
}

// Validate self resolving component
TEST_F(PackChkIntegTests, CheckCompResolvedByItself) {
  const char* argv[3];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/CompResolvedByItself/ARM.CompResolvedByItself.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char *)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(3, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int foundCnt = 0;
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M389")) != string::npos) {
      foundCnt++;
    }
  }

  if (foundCnt != 2) {
    FAIL() << "error: missing message M389";
  }
}

// Validate Option: -n PackName.txt
TEST_F(PackChkIntegTests, CheckPackFileName) {
  const char* argv[5];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/PackNameFile/Arm.PackNameFile_DFP.pdsc";
  string outDir = PackChkIntegTestEnv::testoutput_dir + "/PackFileName";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));
  ASSERT_TRUE(RteFsUtils::CreateDirectories(outDir));

  string packNameFile = outDir;
  packNameFile += (char*)"/PackFileName.txt";

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"-n";
  argv[3] = (char*)packNameFile.c_str();
  argv[4] = (char*)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(5, argv, nullptr));

  ASSERT_TRUE(RteFsUtils::Exists(packNameFile));

  string content;
  getline(std::ifstream(packNameFile), content);

  if(content.compare("Arm.PackNameFile_DFP.0.1.1.pack")) {
    FAIL() << "error: Pack name file must contain 'Arm.PackNameFile_DFP.0.1.1.pack'";
  }
}

// Validate "--allow-suppress-error"
TEST_F(PackChkIntegTests, CheckAllowSuppressError) {
  const char* argv[6];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/AllowSuppressError/TestVendor.TestInvalidPack.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"--allow-suppress-error";
  argv[3] = (char*)"-x";
  argv[4] = (char*)"M323";
  argv[5] = (char*)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(6, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  bool bFound = false;
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M323")) != string::npos) {
      bFound = true;
      break;
    }
  }

  if (bFound) {
    FAIL() << "error: found error M323";
  }
}

// Validate files are inside pack root
TEST_F(PackChkIntegTests, CheckTestPackRoot) {
  const char* argv[2];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/TestPackRoot/Pack/TestVendor.TestPackRoot.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(2, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int foundCnt = 0;
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M313")) != string::npos) {
      foundCnt++;
    }
  }

  if (foundCnt != 1) {
    FAIL() << "error: missing message M313";
  }
}

// Validate invalid file path (file is directory)
TEST_F(PackChkIntegTests, CheckFilenameIsDir) {
  const char *argv[3];

  const string &pdscFile = PackChkIntegTestEnv::localtestdata_dir +
                           "/FilenameIsDir/TestVendor.FilenameIsDirPack.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char *) "";
  argv[1] = (char *) pdscFile.c_str();
  argv[2] = (char *) "--disable-validation";

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(3, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int foundCnt = 0;
  for (const string &msg: errMsgs) {
    size_t s;
    if ((s = msg.find("M356")) != string::npos) {
      foundCnt++;
    }
  }

  if (foundCnt != 1) {
    FAIL() << "error: missing message M356";
  }
}

// Validate "--xsd"
TEST_F(PackChkIntegTests, CheckXsd) {
  const char* argv[4];

  const string& pdscFile = PackChkIntegTestEnv::globaltestdata_dir +
  "/packs/ARM/RteTest/0.1.0/ARM.RteTest.pdsc";
  const string& schemaFile = PackChkIntegTestEnv::localtestdata_dir + "/schema.xsd";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));
  ASSERT_TRUE(RteFsUtils::Exists(schemaFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"--xsd";
  argv[3] = (char*)schemaFile.c_str();

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(4, argv, nullptr));
}

// Validate "--xsd" incorrect path
TEST_F(PackChkIntegTests, CheckNotExistXsd) {
  const char* argv[4];

  const string& pdscFile = PackChkIntegTestEnv::globaltestdata_dir +
                           "/packs/ARM/RteTest/0.1.0/ARM.RteTest.pdsc";
  const string& schemaFile = PackChkIntegTestEnv::localtestdata_dir + "/schemaNotExist.xsd";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));
  ASSERT_FALSE(RteFsUtils::Exists(schemaFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"--xsd";
  argv[3] = (char*)schemaFile.c_str();

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(4, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  bool bFound = false;
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M219")) != string::npos) {
      bFound = true;
      break;
    }
  }

  if (!bFound) {
    FAIL() << "error: missing error M219";
  }
}

TEST_F(PackChkIntegTests, CheckPackNamedXsdNotFound) {
  const char* argv[4];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/SchemaValidation/TestVendor.SchemaValidation.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  DeletePackXsd();

  const string schemaFileName = GetPackXsd();

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"--xsd";
  argv[3] = (char*)schemaFileName.c_str();

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(4, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M219_foundCnt = 0;

  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M219")) != string::npos) {
      M219_foundCnt++;    // follows one M511: <descripton>
    }
  }

  if (M219_foundCnt != 1) {
    FAIL() << "error: missing message M219";
  }
}

// Validate invalid file path (file is directory)
TEST_F(PackChkIntegTests, CheckFileNameHasSpace) {
  const char* argv[3];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/FileNameHasSpace/TestVendor.FileNameHasSpacePack.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char *)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(3, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int foundCnt = 0;
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M314")) != string::npos) {
      foundCnt++;
    }
  }

  if (foundCnt != 9) {
    FAIL() << "error: missing message M314";
  }
}

// Validate invalid file path (file is directory)
TEST_F(PackChkIntegTests, CheckDuplicateFlashAlgo) {
  const char* argv[2];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/DuplicateFlashAlgo/TestVendor.DuplicateFlashAlgo.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(2, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M348_foundCnt = 0;
  int M369_foundCnt = 0;
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M348")) != string::npos) {
      M348_foundCnt++;
    }
    if ((s = msg.find("M369")) != string::npos) {
      M369_foundCnt++;
    }
  }

  if (M348_foundCnt != 2 || M369_foundCnt != 4) {
    FAIL() << "error: missing message M348 or M369";
  }
}

// Validate URLs to start with 'https://'
TEST_F(PackChkIntegTests, CheckUrlForHttp) {
  const char* argv[5];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/TestUrlHttps/TestVendor.TestUrlHttps.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"--disable-validation";
  argv[3] = (char*)"-x";
  argv[4] = (char*)"!M368";

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(2, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M300_foundCnt = 0;
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M300")) != string::npos) {
      M300_foundCnt++;
    }
  }

  if (M300_foundCnt != 8) {
    FAIL() << "error: missing number of M300, found " << M300_foundCnt;
  }
}

// Validate file is .md
TEST_F(PackChkIntegTests, CheckDescriptionOverviewMd) {
  const char* argv[3];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/DescriptionOverview/TestVendor.DescriptionOverviewPack.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(3, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M337_foundCnt = 0;
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M337")) != string::npos) {
      M337_foundCnt++;
    }
  }

  if (M337_foundCnt) {
    FAIL() << "error: message M337 found";
  }
}

// Validate config file vs. include path
TEST_F(PackChkIntegTests, CheckConfigFileInIncludePath) {
  const char* argv[3];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/ConfigFileInIncludePath/TestVendor.ConfigFileInIncludePathPack.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char *)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(3, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M357_foundCnt = 0;
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M357")) != string::npos) {
      M357_foundCnt++;
    }
  }

  if (M357_foundCnt != 5) {
    FAIL() << "error: missing message M357";
  }
}

// Validate config file vs. include path
TEST_F(PackChkIntegTests, CheckTestTextExceedsLength) {
  const char* argv[7];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/TestTextExceedsLength/TestVendor.TestTextExceedsLength.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char *)"--disable-validation";
  argv[3] = (char*)"-x";
  argv[4] = (char*)"!M387";
  argv[5] = (char*)"-x";
  argv[6] = (char*)"!M388";

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(7, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M387_foundCnt = 0;
  int M388_foundCnt = 0;
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M387")) != string::npos) {
      M387_foundCnt++;
    }
    if ((s = msg.find("M388")) != string::npos) {
      M388_foundCnt++;
    }
  }

  if (!M387_foundCnt || !M388_foundCnt) {
    FAIL() << "error: missing message M387 or M388";
  }
}

// Test schema validation
TEST_F(PackChkIntegTests, CheckSchemaValidation) {
  const char* argv[3];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/SchemaValidation/TestVendor.SchemaValidation.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(2, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M510_foundCnt = 0;
  int M511_foundCnt = 0;
  int M306_foundCnt = 0;

  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M306")) != string::npos) {
      M306_foundCnt++;    // follows one M511: <descripton>
    }
    else if ((s = msg.find("M510")) != string::npos) {
      M510_foundCnt++;
    }
    else if ((s = msg.find("M511")) != string::npos) {
      M511_foundCnt++;
    }
  }

  if (M510_foundCnt != 0 || M511_foundCnt != 6 || M306_foundCnt != 1) {
    FAIL() << "error: missing message M510, M511 or M306";
  }
}

// Validate mounted and compatible board devices
TEST_F(PackChkIntegTests, CheckBoardMountedCompatibleDevices) {
  const char* argv[5];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/CheckBoardMountedCompatibleDevices/BulbBoard_BSP/Keil.BulbBoard_BSP.pdsc";
  const string& pdscFileAdd = PackChkIntegTestEnv::localtestdata_dir +
    "/CheckBoardMountedCompatibleDevices/FM0plus_DFP/Keil.FM0plus_DFP.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));
  ASSERT_TRUE(RteFsUtils::Exists(pdscFileAdd));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"-i";
  argv[3] = (char*)pdscFileAdd.c_str();
  argv[4] = (char*)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(5, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M308_foundCnt = 0;
  int M318_foundCnt = 0;
  int M319_foundCnt = 0;
  int M346_foundCnt = 0;
  int M607_foundCnt = 0;
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M308")) != string::npos) {
      M308_foundCnt++;
    }
    if ((s = msg.find("M318")) != string::npos) {
      M318_foundCnt++;
    }
    if ((s = msg.find("M319")) != string::npos) {
      M319_foundCnt++;
    }
    if ((s = msg.find("M346")) != string::npos) {
      M346_foundCnt++;
    }
    if ((s = msg.find("M607")) != string::npos) {
      M607_foundCnt++;
    }
  }

  if (M308_foundCnt != 2 || M318_foundCnt != 2 || M319_foundCnt != 2 || M346_foundCnt != 3 || M607_foundCnt != 1) {
    FAIL() << "error: missing message(s) on check mounted and compatible devices";
  }
}

TEST_F(PackChkIntegTests, CheckBoardMountedCompatibleDevices2) {
  const char* argv[9];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/RteTestBoard/ARM.RteTestBoard.pdsc";
  const string& pdscFileAdd1 = PackChkIntegTestEnv::globaltestdata_dir +
    "/packs/ARM/RteTest/0.1.0/ARM.RteTest.pdsc";
  const string& pdscFileAdd2 = PackChkIntegTestEnv::globaltestdata_dir +
    "/packs/ARM/RteTest_DFP/0.2.0/ARM.RteTest_DFP.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));
  ASSERT_TRUE(RteFsUtils::Exists(pdscFileAdd1));
  ASSERT_TRUE(RteFsUtils::Exists(pdscFileAdd1));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"-i";
  argv[3] = (char*)pdscFileAdd1.c_str();
  argv[4] = (char*)"-i";
  argv[5] = (char*)pdscFileAdd2.c_str();
  argv[6] = (char*)"--disable-validation";
  argv[7] = (char*)"-x";
  argv[8] = (char*)"!M611";

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(9, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M611_foundCnt = 0;
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M611")) != string::npos) {
      M611_foundCnt++;
    }
  }

  if (M611_foundCnt != 1) {
    FAIL() << "error: missing message(s) on check mounted devices: multiple devices found";
  }
}

TEST_F(PackChkIntegTests, CheckSupportCcFiles) {
  const char* argv[3];

  string pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/SupportCcFiles/TestVendor.SupportCcFiles_DFP.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(3, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M337_foundCnt = 0;
  for (const string& msg : errMsgs) {
    if (msg.find("M337", 0) != string::npos) {
      M337_foundCnt++;
    }
  }

  if(!M337_foundCnt) {
    FAIL() << "error: Missing message M337: File with category 'sourceCpp' has wrong extension 'ccc': 'Files/fileWillFail.ccc'";
  }
}

TEST_F(PackChkIntegTests, CheckFileAttributeDeprecated) {
  const char* argv[3];

  string pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/FileAttributeDeprecated/TestVendor.FileAttributeDeprecated.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(3, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M600_foundCnt = 0;
  for (const string& msg : errMsgs) {
    if (msg.find("M600", 0) != string::npos) {
      M600_foundCnt++;
    }
  }

  if(!M600_foundCnt) {
    FAIL() << "error: Missing message M600: deprecated attributes";
  }
}

// Validate invalid file path (file is directory)
TEST_F(PackChkIntegTests, CheckMemoryAttributes) {
  const char* argv[3];

  const string& pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/MemoryAttributes/TestVendor.MemoryAttributes.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(3, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M608_foundCnt = 0;
  int M609_foundCnt = 0;
  int M308_foundCnt = 0;
  int M309_foundCnt = 0;
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M608")) != string::npos) {
      M608_foundCnt++;
    }
    if ((s = msg.find("M609")) != string::npos) {
      M609_foundCnt++;
    }
    if ((s = msg.find("M308")) != string::npos) {
      M308_foundCnt++;
    }
    if ((s = msg.find("M309")) != string::npos) {
      M309_foundCnt++;
    }

  }

  if (M608_foundCnt != 2 || M609_foundCnt != 2 || M308_foundCnt != 3 || M309_foundCnt != 1) {
    FAIL() << "error: missing message(s) on check device memories";
  }
}

TEST_F(PackChkIntegTests, CheckConcurrentComponentFiles) {
  const char* argv[3];

  string pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/ConcurrentComponentFiles/ARM.ConcurrentComponentFiles.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(3, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M610_foundCnt = 0;
  for (const string& msg : errMsgs) {
    if (msg.find("M610", 0) != string::npos) {
      M610_foundCnt++;
    }
  }

  if(M610_foundCnt != 1) {
    FAIL() << "error: Missing message M610: concurrent files in component";
  }
}

TEST_F(PackChkIntegTests, CheckCsolutionTag) {
  const char* argv[3];

  string pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/CsolutionTag/TestVendor.CsolutionTag.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(3, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M110_foundCnt = 0;
  int M500_foundCnt = 0;
  int M601_foundCnt = 0;
  int M323_foundCnt = 0;
  int M326_foundCnt = 0;
  int M332_foundCnt = 0;

  for (const string& msg : errMsgs) {
    if (msg.find("M110", 0) != string::npos) {
      M110_foundCnt++;
    }
    if (msg.find("M500", 0) != string::npos) {
      M500_foundCnt++;
    }
    if (msg.find("M601", 0) != string::npos) {
      M601_foundCnt++;
    }
    if (msg.find("M323", 0) != string::npos) {
      M323_foundCnt++;
    }
    if (msg.find("M326", 0) != string::npos) {
      M326_foundCnt++;
    }
    if (msg.find("M332", 0) != string::npos) {
      M332_foundCnt++;
    }
  }

  if(M110_foundCnt != 1 || M500_foundCnt != 2 || M601_foundCnt != 7 || M323_foundCnt != 2 || M326_foundCnt != 2 || M332_foundCnt != 2 ) {
    FAIL() << "error: Missing messages testing <csolution> tag";
  }
}

TEST_F(PackChkIntegTests, CheckConditionComponentDependency_Pos) {
  const char* argv[7];

  const string& pdscFile = PackChkIntegTestEnv::globaltestdata_dir +
    "/packs/ARM/RteTest_DFP/0.2.0/ARM.RteTest_DFP.pdsc";
  const string& refFile = PackChkIntegTestEnv::globaltestdata_dir +
    "/packs/ARM/RteTest/0.1.0/ARM.RteTest.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));
  ASSERT_TRUE(RteFsUtils::Exists(refFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"-i";
  argv[3] = (char*)refFile.c_str();
  argv[4] = (char*)"-x";
  argv[5] = (char*)"!M317";
  argv[6] = (char*)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(7, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M317_foundCnt = 0;
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M317", 0)) != string::npos) {
      M317_foundCnt++;
    }
  }

  if(M317_foundCnt) {
    FAIL() << "error: warning M317 found";
  }
}

TEST_F(PackChkIntegTests, CheckProcessorFeatures) {
  const char* argv[3];

  string pdscFile = PackChkIntegTestEnv::localtestdata_dir +
    "/ProcessorFeatures/TestVendor.ProcessorFeatures.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(1, packChk.Check(3, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M604_foundCnt = 0;
  int M605_foundCnt = 0;
  for (const string& msg : errMsgs) {
    if (msg.find("M604", 0) != string::npos) {
      M604_foundCnt++;
    }
    if (msg.find("M605", 0) != string::npos) {
      M605_foundCnt++;
    }
  }

  if(M604_foundCnt != 1 || M605_foundCnt != 98) {
    FAIL() << "error: Missing message M604, M605: processor features";
  }
}

TEST_F(PackChkIntegTests, CheckConditionComponentDependency_Neg) {
  const char* argv[5];

  const string& pdscFile = PackChkIntegTestEnv::globaltestdata_dir +
    "/packs/ARM/RteTest_DFP/0.2.0/ARM.RteTest_DFP.pdsc";
  const string& refFile = PackChkIntegTestEnv::globaltestdata_dir +
    "/packs/ARM/RteTest/0.1.0/ARM.RteTest.pdsc";
  ASSERT_TRUE(RteFsUtils::Exists(pdscFile));
  ASSERT_TRUE(RteFsUtils::Exists(refFile));

  argv[0] = (char*)"";
  argv[1] = (char*)pdscFile.c_str();
  argv[2] = (char*)"-x";
  argv[3] = (char*)"!M317";
  argv[4] = (char*)"--disable-validation";

  PackChk packChk;
  EXPECT_EQ(0, packChk.Check(5, argv, nullptr));

  auto errMsgs = ErrLog::Get()->GetLogMessages();
  int M317_foundCnt = 0;
  for (const string& msg : errMsgs) {
    size_t s;
    if ((s = msg.find("M317", 0)) != string::npos) {
      M317_foundCnt++;
    }
  }

  if(M317_foundCnt != 4) {
    FAIL() << "error: warning M317 count != 4";
  }
}

