/*
 * Copyright (c) 2020-2025 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PROJMGRWORKER_H
#define PROJMGRWORKER_H

#include "ProjMgrExtGenerator.h"
#include "ProjMgrKernel.h"
#include "ProjMgrParser.h"
#include "ProjMgrUtils.h"

/**
 * Forward declarations
*/
class ProjMgrYamlEmitter;

/**
 * @brief connections validation result containing
 *        boolean valid,
 *        conflicted connections,
 *        overflowed connections,
 *        incompatible connections,
 *        missed provided combined connections,
 *        active connect map,
*/
struct ConnectionsValidationResult {
  bool valid;
  StrVec conflicts;
  StrPairVec overflows;
  StrPairVec incompatibles;
  std::vector<ConnectionsCollection> missedCollections;
  ActiveConnectMap activeConnectMap;
};

/**
 * @brief layers discovering variables
 *        required layer types,
 *        missed required types,
 *        optional type flags,
 *        generic clayers from search path,
 *        generic clayers from packs,
 *        candidate layers,
*/
struct LayersDiscovering {
  StrVec requiredLayerTypes;
  StrVec missedRequiredTypes;
  BoolMap optionalTypeFlags;
  StrVecMap genericClayersFromSearchPath;
  StrVecMap genericClayersFromPacks;
  StrVecMap candidateClayers;
};

/**
 * @brief connections lists
 *        list of consumes
 *        list of provides
*/
struct ConnectionsList {
  StrPairPtrVec consumes;
  StrPairPtrVec provides;
};

/**
 * @brief Environment list
 *        cmsis_pack_root,
 *        cmsis_compiler_root
*/
struct EnvironmentList {
  std::string cmsis_pack_root;
  std::string cmsis_compiler_root;
};

/**
 * @brief toolchain item containing
 *        toolchain name,
 *        toolchain version,
 *        toolchain required version,
 *        toolchain required range in the format <min>[:<max>],
 *        toolchain root,
 *        toolchain config
*/
struct ToolchainItem {
  std::string name;
  std::string version;
  std::string required;
  std::string range;
  std::string root;
  std::string config;
};

/**
 * @brief package item containing
 *        pack information pack,
 *        path to pack path,
 *        origin file,
*/
struct PackageItem {
  PackInfo    pack;
  std::string path;
  std::string origin;
};

/**
 * @brief device item containing
 *        device name,
 *        device vendor,
 *        device processor name,
*/
struct DeviceItem {
  std::string vendor;
  std::string name;
  std::string pname;
};

/**
 * @brief board item containing
 *        board vendor,
 *        board name,
*/
struct BoardItem {
  std::string vendor;
  std::string name;
  std::string revision;
  bool operator==(const BoardItem& b) const {
    return name == b.name && revision == b.revision;
  }
};

/**
 * @brief target item containing
 *        target-type board,
 *        target-type device name,
*/
struct TargetItem {
  std::string board;
  std::string device;
};

/**
 * @brief selected component item containing
 *        pointer to component instance
 *        pointer to input component item
 *        generator identifier
*/
struct SelectedComponentItem {
  RteComponentInstance* instance;
  ComponentItem* item;
  std::string generator;
};

/**
 * @brief component file item containing
 *        file name
 *        file attribute
 *        file category
 *        file language
 *        file scope
 *        file version
 *        file select
*/
struct ComponentFileItem {
  std::string name;
  std::string attr;
  std::string category;
  std::string language;
  std::string scope;
  std::string version;
  std::string select;
};

/**
 * @brief gpdsc item containing
 *        component id
 *        generator id
 *        working directory
*/
struct GpdscItem {
  std::string component;
  std::string generator;
  std::string workingDir;
};

/**
 * @brief translation control item containing
 *        final context controls after processing,
 *        csolution controls,
 *        cproject controls,
 *        target-type controls,
 *        build-type controls,
 *        list of project setup controls,
 *        map of layers translation controls,
*/
struct TranslationControl {
  BuildType processed;
  BuildType csolution;
  BuildType cproject;
  BuildType target;
  BuildType build;
  std::vector<BuildType> setups;
  std::map<std::string, BuildType> clayers;
};

/**
 * @brief linker context item containing
 *        auto linker script generation
 *        list of linker scripts for processing,
 *        list of linker regions files for processing,
 *        list of defines
 *        processed regions file,
 *        processed script file
*/
struct LinkerContextItem {
  bool autoGen;
  StrVec scriptList;
  StrVec regionsList;
  StrVec defines;
  std::string regions;
  std::string script;
};

/**
 * @brief book item containing
 *        string name
 *        string title
 *        string category
*/
struct BookItem {
  std::string name;
  std::string title;
  std::string category;
};

/**
 * @brief context type item containing
 *        list of all build types including mapped ones
 *        list of all target types including mapped ones
 *        map of missing build types
 *        map of missing target types
*/
struct ContextTypesItem {
  StrVec allBuildTypes;
  StrVec allTargetTypes;
  BoolMap missingBuildTypes;
  BoolMap missingTargetTypes;
};

/**
 * @brief validation condition item containing
 *        expression (accept/deny/require ...)
 *        related aggregates
*/
struct ValidationCondition {
  std::string expression;
  StrSet aggregates;
};

/**
 * @brief validation result containing
 *        result according to enum ConditionResult
 *        component/api identifier
 *        direct related aggregates
 *        conditions (expressions and related identifiers)
*/

struct ValidationResult {
  RteItem::ConditionResult result;
  std::string id;
  StrSet aggregates;
  std::vector<ValidationCondition> conditions;
};

/**
 * @brief gdb server item containing
 *        port number of processor
 *        processor name
*/
struct GdbServerItem {
  unsigned long long port;
  std::string pname;
};

/**
 * @brief example environment item containing
 *        project file with extension
 *        subdirectory to be copied
*/
struct EnvironmentItem {
  std::string load;
  std::string folder;
  bool operator==(const EnvironmentItem& b) const { return true; }
};

/**
 * @brief example item containing
 *        name of example
 *        description
 *        documentation
 *        version
 *        archive
 *        map of environments
 *        list of compatible boards
 *        list of components
 *        list of categories
 *        list of keywords
 *        pack identifier
*/
struct ExampleItem {
  std::string name;
  std::string description;
  std::string doc;
  std::string version;
  std::string archive;
  std::map<std::string, EnvironmentItem> environments;
  std::vector<BoardItem> boards;
  std::vector<std::string> components;
  std::vector<std::string> categories;
  std::vector<std::string> keywords;
  std::string pack;
  bool operator==(const ExampleItem& e) const {
    return name == e.name && version == e.version && description == e.description &&
      boards == e.boards && environments == e.environments;
  }
};

/**
 * @brief template item containing
 *        name of example
 *        description
 *        path to the directory that contains the template
 *        path to the *.csolution.yml file
 *        path to copy the template into
 *        pack identifier
*/
struct TemplateItem {
  std::string name;
  std::string description;
  std::string path;
  std::string file;
  std::string copyTo;
  std::string pack;
};

/**
 * @brief debugger type
 *        name of debug configuration
 *        brief description
 *        debug protocol (jtag or swd)
 *        debug clock speed
 *        debug configuration file
 *        start pname
 *        list of gdbserver items
 *        custom options
*/
struct DebuggerType {
  std::string name;
  std::string info;
  std::string protocol;
  std::optional<unsigned long long> clock;
  std::string dbgconf;
  std::string startPname;
  std::vector<GdbServerItem> gdbserver;
  CustomItem custom;
};

/**
 * @brief project context item containing
 *        pointer to csolution,
 *        pointer to cproject,
 *        map of pointers to clayers,
 *        pointer to rte project,
 *        pointer to rte target,
 *        pointer to rte filtered model,
 *        pointer to rte components,
 *        pointer to rte device,
 *        pointer to rte board,
 *        translation controls,
 *        target-type item,
 *        parent csolution target properties,
 *        directories,
 *        build-type/target-type pair,
 *        project name,
 *        project description,
 *        list of output files,
 *        output type,
 *        device selection,
 *        board selection,
 *        device item struct,
 *        board item struct,
 *        list of package requirements,
 *        map of required pdsc files and optionally its local path
 *        list of component requirements,
 *        compiler string in short syntax,
 *        toolchain with parsed name and version,
 *        map of target attributes,
 *        map of used packages,
 *        map of used components,
 *        map of unresolved dependencies,
 *        map of config files,
 *        map of PLM status,
 *        list of user groups,
 *        map of absolute file paths,
 *        map of generators,
 *        map of compatible layers,
 *        map of layer variables,
 *        valid connections,
 *        linker options,
 *        map of variables,
 *        external generator directory,
 *        device pack,
 *        board pack,
 *        boolean processed precedences
 *        map of user inputed pack ID to resolved pack ID
 *        set of absolute file paths of project local packs
 *        vector of dependent contexts
 *        map of layers descriptors from packs
 *        flag indicating the context needs a rebuild
 *        vector of device books
 *        vector of board books
 *        additional memory
 *        debugger
 *        default dbgconf
 *        images
 *        selected target-set
 *        load offset for generated binary
 *        elf load mode
 *        image only flag
*/
struct ContextItem {
  CdefaultItem* cdefault = nullptr;
  CsolutionItem* csolution = nullptr;
  CprojectItem* cproject = nullptr;
  std::map<std::string, ClayerItem*> clayers;
  RteProject* rteActiveProject = nullptr;
  RteTarget* rteActiveTarget = nullptr;
  RteModel* rteFilteredModel = nullptr;
  RteItem* rteComponents = nullptr;
  RteDeviceItem* rteDevice = nullptr;
  RteBoard* rteBoard = nullptr;
  TranslationControl controls;
  TargetItem targetItem;
  DirectoriesItem directories;
  TypePair type;
  std::string name;
  std::string description;
  OutputTypes outputTypes;
  std::string device;
  std::string board;
  DeviceItem deviceItem;
  BoardItem boardItem;
  std::vector<PackageItem> packRequirements;
  std::map<std::string, std::pair<std::string, std::string>> pdscFiles;
  std::vector<PackInfo> missingPacks;
  StrVec unusedPacks;
  std::vector<std::pair<ComponentItem, std::string>> componentRequirements;
  std::string compiler;
  ToolchainItem toolchain;
  std::map<std::string, std::string> targetAttributes;
  std::map<std::string, RtePackage*> packages;
  std::map<std::string, SelectedComponentItem> components;
  std::map<std::string, std::pair<RteApi*, std::vector<std::string>>> apis;
  std::map<std::string, SelectedComponentItem> bootstrapComponents;
  StrMap bootstrapMap;
  std::vector<ValidationResult> validationResults;
  std::map<std::string, std::map<std::string, RteFileInstance*>> configFiles;
  std::map<std::string, std::string> plmStatus;
  std::map<std::string, std::vector<ComponentFileItem>> componentFiles;
  std::map<std::string, std::vector<ComponentFileItem>> apiFiles;
  std::map<std::string, std::vector<ComponentFileItem>> generatorInputFiles;
  std::vector<GroupNode> groups;
  std::map<std::string, RteGenerator*> generators;
  std::map<std::string, GpdscItem> gpdscs;
  StrVecMap compatibleLayers;
  StrMap layerVariables;
  std::vector<ConnectionsCollectionVec> validConnections;
  LinkerContextItem linker;
  std::map<std::string, std::string> variables;
  std::map<std::string, GeneratorOptionsItem> extGen;
  RtePackage* devicePack = nullptr;
  RtePackage* boardPack = nullptr;
  bool precedences;
  std::map<std::string, std::set<std::string>> userInputToResolvedPackIdMap;
  StrSet localPackPaths;
  StrVec dependsOn;
  std::map<std::string, RteItem*> packLayers;
  bool needRebuild = false;
  std::vector<BookItem> deviceBooks;
  std::vector<BookItem> boardBooks;
  std::vector<MemoryItem> memory;
  DebuggerType debugger;
  std::pair<std::string, RteFileInstance*> dbgconf;
  std::vector<ImageItem> images;
  std::string targetSet;
  std::string loadOffset;
  std::string elfLoadMode;
  bool imageOnly = false;
};

/**
  * @brief plm status
*/
static constexpr const char* PLM_STATUS_MISSING_FILE = "missing file";
static constexpr const char* PLM_STATUS_MISSING_BASE = "missing base";
static constexpr const char* PLM_STATUS_UPDATE_REQUIRED = "update required";
static constexpr const char* PLM_STATUS_UPDATE_RECOMMENDED = "update recommended";
static constexpr const char* PLM_STATUS_UPDATE_SUGGESTED = "update suggested";

/**
  * @brief set policy for packs loading [latest|all|required]
*/
enum class LoadPacksPolicy
{
  DEFAULT = 0,
  LATEST,
  ALL,
  REQUIRED
};

/**
  * @brief Message Type
*/
enum class MessageType {
  Warning,
  Error
};

/**
  * @brief Process Board/Device
*/
enum class BoardOrDevice {
  Both,
  None,
  SkipDevice,
  SkipProcessor,
};

/**
 * @brief projmgr worker class responsible for processing requests and orchestrating parser and generator calls
*/
class ProjMgrWorker {
public:
  /**
   * @brief class constructor
  */
  ProjMgrWorker(ProjMgrParser* parser, ProjMgrExtGenerator* extGenerator);

  /**
   * @brief class destructor
  */
  ~ProjMgrWorker(void);

  /**
   * @brief process context
   * @param reference to context
   * @param loadGenFiles boolean automatically load generated files, default true
   * @param resolveDependencies boolean automatically resolve dependencies, default true
   * @param updateRteFiles boolean update RTE files, default true
   * @return true if executed successfully
  */
  bool ProcessContext(ContextItem& context, bool loadGenFiles = true, bool resolveDependencies = true, bool updateRteFiles = true);

  /**
   * @brief list available packs
   * @param reference to list of packs
   * @param bListMissingPacksOnly only missing packs
   * @param filter words to filter results
   * @return true if executed successfully
  */
  bool ListPacks(std::vector<std::string>& packs, bool bListMissingPacksOnly, const std::string& filter = RteUtils::EMPTY_STRING);

  /**
   * @brief list available boards
   * @param reference to list of boards
   * @param filter words to filter results
   * @return true if executed successfully
  */
  bool ListBoards(std::vector<std::string>& boards, const std::string& filter = RteUtils::EMPTY_STRING);

  /**
   * @brief list available devices
   * @param reference to list of devices
   * @param filter words to filter results
   * @return true if executed successfully
  */
  bool ListDevices(std::vector<std::string>& devices, const std::string& filter = RteUtils::EMPTY_STRING);

  /**
   * @brief list available components
   * @param reference to list of components
   * @param filter words to filter results
   * @return true if executed successfully
  */
  bool ListComponents(std::vector<std::string>& components, const std::string& filter = RteUtils::EMPTY_STRING);

  /**
   * @brief list configuration files
   * @param reference to list of file description strings
   * @param filter words to filter results
   * @return true if executed successfully
  */
  bool ListConfigs(std::vector<std::string>& dependencies, const std::string& filter = RteUtils::EMPTY_STRING);

  /**
   * @brief list available dependencies
   * @param reference to list of dependencies
   * @param filter words to filter results
   * @return true if executed successfully
  */
  bool ListDependencies(std::vector<std::string>& dependencies, const std::string& filter = RteUtils::EMPTY_STRING);

  /**
   * @brief list available examples
   * @param reference to list of examples
   * @param filter words to filter results
   * @return true if executed successfully
  */
  bool ListExamples(std::vector<std::string>& examples, const std::string& filter = RteUtils::EMPTY_STRING);

  /**
   * @brief list available csolution templates
   * @param reference to list of csolution templates
   * @param filter words to filter results
   * @return true if executed successfully
  */
  bool ListTemplates(std::vector<std::string>& templates, const std::string& filter = RteUtils::EMPTY_STRING);

  /**
   * @brief list contexts
   * @param reference to list of contexts
   * @param filter words to filter results
   * @return true if executed successfully
  */
  bool ListContexts(std::vector<std::string>& contexts, const std::string& filter = RteUtils::EMPTY_STRING, const bool ymlOrder = false);

  /**
   * @brief list target-sets
   * @param reference to list of target-sets
   * @param filter words to filter results
   * @return true if executed successfully
  */
  bool ListTargetSets(std::vector<std::string>& targetSets, const std::string& filter = RteUtils::EMPTY_STRING);

  /**
   * @brief list generators of a given context
   * @param reference to list of generators
   * @return true if executed successfully
  */
  bool ListGenerators(std::vector<std::string>& generators);

  /**
  * @brief list available, referenced or compatible layers
  * @param reference to list of layers
  * @param reference clayer search path
  * @param reference to failed contexts set
  * @return true if executed successfully
  */
  bool ListLayers(std::vector<std::string>& layers, const std::string& clayerSearchPath, StrSet& failedContexts);

  /**
  * @brief list installed toolchains
  * @param reference to vector of toolchain items
  * @return true if executed successfully
  */
  bool ListToolchains(std::vector<ToolchainItem>& toolchains);

  /**
  * @brief list environment configurations
  * @param reference to EnvironmentList struct
  * @return true if executed successfully
  */
  bool ListEnvironment(EnvironmentList& env);

  /**
  * @brief list config files
  * @param reference to vector of config files info
  * @return true if executed successfully
  */
  bool ListConfigFiles(std::vector<std::string>& configFiles);

  /**
   * @brief add contexts for a given descriptor
   * @param reference to parser
   * @param reference to descriptor
   * @param cproject filename
   * @return true if executed successfully
  */
  bool AddContexts(ProjMgrParser& parser, ContextDesc& descriptor, const std::string& cprojectFile);

  /**
   * @brief get context map
   * @param pointer to context map
  */
  void GetContexts(std::map<std::string, ContextItem>* &contexts);

  /**
   * @brief get yml ordered contexts
   * @param reference to contexts
  */
  void GetYmlOrderedContexts(std::vector<std::string> &contexts);

  /**
   * @brief get executes node at solution level
   * @param reference to executes map
  */
  void GetExecutes(std::map<std::string, ExecutesItem>& executes);

  /**
   * @brief set output directory
   * @param reference to output directory
  */
  void SetOutputDir(const std::string& outputDir);

  /**
   * @brief update csolution tmp directory
  */
  void UpdateTmpDir();

  /**
   * @brief set root directory (csolution directory)
   * @param reference to root directory
  */
  void SetRootDir(const std::string& rootDir);

  /**
   * @brief set csolution file
   * @param reference to absolute csolution.yml filename
  */
  void SetCsolutionFile(const std::string& csolutionFile);

  /**
   * @brief set check schema
   * @param boolean check schema
  */
  void SetCheckSchema(bool checkSchema);

  /**
   * @brief set verbose mode
   * @param boolean verbose
  */
  void SetVerbose(bool verbose);

  /**
   * @brief set debug verbosity
   * @param boolean debug
  */
  void SetDebug(bool debug);

  /**
   * @brief set dry-run mode
   * @param boolean dryRun
  */
  void SetDryRun(bool dryRun);

  /**
   * @brief set cbuild2cmake mode
   * @param boolean cbuild2cmake
  */
  void SetCbuild2Cmake(bool cbuild2cmake);

  /**
   * @brief set printing paths relative to project or ${CMSSIS_PACK_ROOT}
   * @param boolean bRelativePaths
  */
  void SetPrintRelativePaths(bool bRelativePaths);

  /**
   * @brief set load packs policy
   * @param reference to load packs policy
  */
  void SetLoadPacksPolicy(const LoadPacksPolicy& policy);

  /**
   * @brief set vector of environment variables
   * @param reference to vector of environment variables
  */
  void SetEnvironmentVariables(const StrVec& envVars);

  /**
   * @brief set selected toolchain
   * @param reference to selected toolchain
  */
  void SetSelectedToolchain(const std::string& selectedToolchain);

  /**
   * @brief set flag when setup command is triggered
   * @param boolean isSetup
  */
  void SetUpCommand(bool isSetup);

  /**
   * @brief set flag when running in rpc mode
   * @param boolean rpcMode
  */
  void RpcMode(bool rpcMode);

  /**
   * @brief set yaml emitter
   * @param pointer to yaml emitter
  */
  void SetEmitter(ProjMgrYamlEmitter* emitter);

  /**
   * @brief execute generator of a given context
   * @param generator identifier
   * @return true if executed successfully
  */
  bool ExecuteGenerator(std::string& generatorId);

  /**
   * @brief execute external generator of a given context
   * @param generator identifier
   * @return true if executed successfully
  */
  bool ExecuteExtGenerator(std::string& generatorId);

  /**
   * @brief initialize model
   * @return true if executed successfully
  */
  bool InitializeModel(void);

  /**
   * @brief initialize rte target for a given context
   * @param context reference
   * @return true if executed successfully
  */
  bool InitializeTarget(ContextItem& context);

  /**
   * @brief load all relevant packs
   * @return true if executed successfully
  */
  bool LoadAllRelevantPacks(void);

  /**
   * @brief parse context selection
   * @param contexts pattern (wildcards are allowed)
   * @param check cbuildset flag (default false)
   * @return true if executed successfully
  */
  bool ParseContextSelection(
    const std::vector<std::string>& contextSelection,
    const bool checkCbuildSet = false);

  /**
   * @brief get the list of selected contexts
   * @return list of selected contexts
  */
  std::vector<std::string> GetSelectedContexts() const;

  /**
   * @brief check if context is selected
   * @param context name
   * @return true if it is selected
  */
  bool IsContextSelected(const std::string& context);

  /**
   * @brief get compiler root directory
   * @return string compiler root directory
  */
  std::string GetCompilerRoot(void);

  /**
   * @brief get pack root directory
   * @return string pack root directory
  */
  std::string GetPackRoot(void);

  /**
   * @brief retrieve all context types, including mapped ones
  */
  void RetrieveAllContextTypes(void);

  /**
   * @brief print missing filters
  */
  void PrintMissingFilters(void);

  /**
   * @brief get selected toolchain
   * @return string selected toolchain
  */
  std::string GetSelectedToochain(void);

  /**
   * @brief get selectable compilers
   * @return string vector reference to selectable compilers
  */
  const StrVec& GetSelectableCompilers(void);

  /**
   * @brief process global generators for a given context
   * @param context selected context item
   * @param generator identifier
   * @param reference to project type
   * @param reference to sibling contexts
   * @return true if executed successfully
  */
  bool ProcessGlobalGenerators(ContextItem* context, const std::string& generatorId,
    std::string& projectType, StrVec& siblings);

  /**
   * @brief check whether variable definition error is set
   * @return true if error is set
  */
  bool HasVarDefineError();

  /**
   * @brief check whether compiler is not defined and there are selectable ones
   * @return true if error is set
  */
  bool HasCompilerDefineError();

  /**
   * @brief get list of detected undefined layer variables
   * @return list of undefined layer variables
  */
  const std::set<std::string>& GetUndefLayerVars();

  /**
   * @brief process solution level executes
   * @return true if it is processed successfully
  */
  bool ProcessSolutionExecutes();

  /**
   * @brief process executes nodes dependencies
  */
  void ProcessExecutesDependencies();

  /**
    * @brief validates and restrict the input contexts
    * @param contexts list to be validated
    * @param fromCbuildSet true is the contexts are read from cbuild-set, otherwise false
    * @return true if validation success
  */
  bool ValidateContexts(const std::vector<std::string>& contexts, bool fromCbuildSet);

  /**
   * @brief check and print if project has toolchain errors
   * @return true if errors
  */
  bool HasToolchainErrors();

  /**
   * @brief check config PLM files
   * @param reference to context
   * @return true if check success
  */
  bool CheckConfigPLMFiles(ContextItem& context);

  /**
   * @brief check user missing files
   * @return true if check success
  */
  bool CheckMissingFiles();

  /**
   * @brief collect unused packs for each selected context
  */
  void CollectUnusedPacks();

  /**
   * @brief check rte errors
   * @return true if there is no error
  */
  bool CheckRteErrors(void);

  /**
   * @brief load packs
   * @param context item
   * @return true if there is no error
  */
  bool LoadPacks(ContextItem& context);

  /**
   * @brief set target attributes
   * @param context item
   * @param map of attributes
   * @return true if there is no error
  */
  bool SetTargetAttributes(ContextItem& context, std::map<std::string, std::string>& attributes);

  /**
   * @brief add required components
   * @param context item
   * @return true if there is no error
  */
  bool AddRequiredComponents(ContextItem& context);

  /**
   * @brief validate context
   * @param context item
   * @return true if there is no error
  */
  RteItem::ConditionResult ValidateContext(ContextItem& context);

  /**
   * @brief populate active target set
   * @param active target set command line option
   * @return true if there is no error
  */
  bool PopulateActiveTargetSet(const std::string& activeTargetSet);

  /**
   * @brief add image-only context
  */
  void AddImageOnlyContext();

  /**
   * @brief process device and/or board
   * @param context item
   * @param scope: process board, device or both
   * @return true if there is no error
  */
  bool ProcessDevice(ContextItem& context, BoardOrDevice process = BoardOrDevice::Both);

  /**
   * @brief collect examples
   * @param context item
   * @param environments filter 
   * @return vector of example items
  */
  std::vector<ExampleItem> CollectExamples(const ContextItem& context, const StrVec& filter);

  /**
   * @brief collect templates
   * @param context item
   * @return vector of template items
  */
  std::vector<TemplateItem> CollectTemplates(const ContextItem& context);

  /**
   * @brief clear worker members for reloading a solution
   * @return true if there is no error
  */
  void Clear() {
    for (auto context : m_contexts) {
      for (auto componentItem : context.second.components) {
        delete componentItem.second.instance;
      }
    }
    m_contexts.clear();
    m_ymlOrderedContexts.clear();
    m_contexts.clear();
    m_contextErrMap.clear();
    m_selectedContexts.clear();
    m_outputDir.clear();
    m_selectedToolchain.clear();
    m_rootDir.clear();
    m_undefLayerVars.clear();
    m_packMetadata.clear();
    m_executes.clear();
    m_toolchainErrors.clear();
    m_selectableCompilers.clear();
    m_missingFiles.clear();
    m_types = {};
  };

protected:
  ProjMgrParser* m_parser = nullptr;
  ProjMgrKernel* m_kernel = nullptr;
  RteGlobalModel* m_model = nullptr;
  ProjMgrExtGenerator* m_extGenerator = nullptr;
  ProjMgrYamlEmitter* m_emitter = nullptr;
  std::list<RtePackage*> m_loadedPacks;
  std::vector<ToolchainItem> m_toolchains;
  StrVec m_toolchainConfigFiles;
  StrVec m_missingToolchains;
  StrVec m_envVars;
  std::map<std::string, StrMap> m_regToolchainsEnvVars;
  std::vector<std::string> m_ymlOrderedContexts;
  std::map<std::string, ContextItem> m_contexts;
  std::map<std::string, std::set<std::string>> m_contextErrMap;
  std::vector<std::string> m_selectedContexts;
  std::string m_outputDir;
  std::string m_packRoot;
  std::string m_compilerRoot;
  std::string m_selectedToolchain;
  std::string m_rootDir;
  std::string m_csolutionFile;
  LoadPacksPolicy m_loadPacksPolicy;
  ContextTypesItem m_types;
  bool m_checkSchema;
  bool m_verbose;
  bool m_debug;
  bool m_dryRun;
  bool m_relativePaths;
  bool m_cbuild2cmake;
  bool m_isSetupCommand;
  bool m_rpcMode = false;
  std::set<std::string> m_undefLayerVars;
  StrMap m_packMetadata;
  std::map<std::string, ExecutesItem> m_executes;
  std::map<MessageType, StrSet> m_toolchainErrors;
  StrVec m_selectableCompilers;
  bool m_undefCompiler = false;
  std::map<std::string, FileNode> m_missingFiles;
  std::string m_activeTargetType;
  TargetSetItem m_activeTargetSet;
  CprojectItem m_imageOnly;

  bool CheckMissingPackRequirements(const std::string& contextName);
  void CheckMissingLinkerScript(ContextItem& context);
  bool CollectRequiredPdscFiles(ContextItem& context, const std::string& packRoot);
  bool CheckBoardDeviceInLayer(const ContextItem& context, const ClayerItem& clayer);
  bool CheckCompiler(const std::vector<std::string>& forCompiler, const std::string& selectedCompiler);
  bool CheckType(const TypeFilter& typeFilter, const std::vector<TypePair>& typeVec);
  bool CheckContextFilters(const TypeFilter& typeFilter, const ContextItem& context);
  bool GetTypeContent(ContextItem& context);
  bool GetProjectSetup(ContextItem& context);
  bool ProcessPrecedences(ContextItem& context, BoardOrDevice process = BoardOrDevice::None, bool rerun = false);
  bool ProcessPrecedence(StringCollection& item);
  bool ProcessCompilerPrecedence(StringCollection& item, bool acceptRedefinition = false);
  bool ProcessDevicePrecedence(StringCollection& item);
  bool ProcessBoardPrecedence(StringCollection& item);
  bool ProcessToolchain(ContextItem& context);
  bool ProcessPackages(ContextItem& context, const std::string& packRoot);
  bool ProcessComponents(ContextItem& context);
  RteComponentInstance* ProcessComponent(ContextItem& context, ComponentItem& item, RteComponentMap& componentMap, std::string& hint);
  RteComponent* ResolveComponent(RteComponentInstance* ci, ContextItem& context, ComponentItem& item, RteComponentMap& componentMap, std::string& hint);
  bool ProcessGpdsc(ContextItem& context);
  bool ProcessConfigFiles(ContextItem& context);
  bool ProcessComponentFiles(ContextItem& context);
  bool ProcessDebuggers(ContextItem& context);
  bool ProcessImages(ContextItem& context);
  bool ProcessExecutes(ContextItem& context, bool solutionLevel = false);
  bool ProcessGroups(ContextItem& context);
  bool ProcessSequencesRelatives(ContextItem& context, bool rerun);
  bool ProcessSequencesRelatives(ContextItem& context, std::vector<std::string>& src, const std::string& ref = std::string(), std::string outDir = std::string(), bool withHeadingDot = false, bool solutionLevel = false);
  bool ProcessSequencesRelatives(ContextItem& context, BuildType& build, const std::string& ref = std::string());
  bool ProcessSequenceRelative(ContextItem& context, std::string& item, const std::string& ref = std::string(), std::string outDir = std::string(), bool withHeadingDot = false, bool solutionLevel = false);
  bool ProcessOutputFilenames(ContextItem& context);
  bool ProcessLinkerOptions(ContextItem& context);
  bool ProcessLinkerOptions(ContextItem& context, const LinkerItem& linker, const std::string& ref);
  bool ProcessProcessorOptions(ContextItem& context);
  void AddContext(ContextDesc& descriptor, const TypePair& type, ContextItem& parentContext);
  bool FormatValidationResults(std::set<std::string>& results, const ContextItem& context);
  void UpdateMisc(std::vector<MiscItem>& vec, const std::string& compiler);
  void AddMiscUniquely(MiscItem& dst, std::vector<std::vector<MiscItem>*>& srcVec);
  void AddMiscUniquely(MiscItem& dst, std::vector<MiscItem>& srcVec);
  bool AddGroup(const GroupNode& src, std::vector<GroupNode>& dst, ContextItem& context, const std::string root);
  bool AddFile(const FileNode& src, std::vector<FileNode>& dst, ContextItem& context, const std::string root);
  bool AddComponent(const ComponentItem& src, const std::string& layer, std::vector<std::pair<ComponentItem, std::string>>& dst, TypePair type, ContextItem& context);
  void GetDeviceItem(const std::string& element, DeviceItem& device) const;
  void GetBoardItem (const std::string& element, BoardItem& board) const;
  bool GetPrecedentValue(std::string& outValue, const std::string& element) const;
  std::string GetDeviceInfoString(const std::string& vendor, const std::string& name, const std::string& processor) const;
  std::string GetBoardInfoString(const std::string& vendor, const std::string& name, const std::string& revision) const;
  std::vector<PackageItem> GetFilteredPacks(const PackageItem& packItem, const std::string& rtePath) const;
  ToolchainItem GetToolchain(const std::string& compiler);
  bool IsPreIncludeByTarget(const RteTarget* activeTarget, const std::string& preInclude);
  void PrintConnectionsValidation(ConnectionsValidationResult result, std::string& msg);
  bool CollectLayersFromPacks(ContextItem& context, StrVecMap& clayers);
  bool CollectLayersFromSearchPath(const std::string& clayerSearchPath, StrVecMap& clayers);
  void GetRequiredLayerTypes(ContextItem& context, LayersDiscovering& discover);
  bool GetCandidateLayers(LayersDiscovering& discover);
  bool ProcessLayerCombinations(ContextItem& context, LayersDiscovering& discover);
  bool DiscoverMatchingLayers(ContextItem& context, std::string clayerSearchPath);
  void CollectConnections(ContextItem& context, ConnectionsCollectionVec& connections);
  void GetActiveConnectMap(const ConnectionsCollectionVec& collection, ActiveConnectMap& activeConnectMap);
  void SetActiveConnect(const ConnectItem* activeConnect, const ConnectionsCollectionVec& collection, ActiveConnectMap& activeConnectMap);
  ConnectionsCollectionMap ClassifyConnections(const ConnectionsCollectionVec& connections, BoolMap optionalTypeFlags);
  ConnectionsValidationResult ValidateConnections(ConnectionsCollectionVec combination);
  void GetAllCombinations(const ConnectionsCollectionMap& src, const ConnectionsCollectionMap::iterator& it,
    std::vector<ConnectionsCollectionVec>& combinations, const ConnectionsCollectionVec& previous = ConnectionsCollectionVec());
  void GetAllSelectCombinations(const ConnectPtrMap& src, const ConnectPtrMap::iterator& it,
    std::vector<ConnectPtrVec>& combinations, const ConnectPtrVec& previous = ConnectPtrVec());
  void PushBackUniquely(ConnectionsCollectionVec& vec, const ConnectionsCollection& value);
  void PushBackUniquely(std::vector<ToolchainItem>& vec, const ToolchainItem& value);
  void GetRegisteredToolchainEnvVars(void);
  void GetRegisteredToolchains(void);
  bool GetLatestToolchain(ToolchainItem& toolchain);
  bool GetToolchainConfig(const std::string& name, const std::string& version, std::string& configPath, std::string& selectedConfigVersion);
  bool IsConnectionSubset(const ConnectionsCollection& connectionSubset, const ConnectionsCollection& connectionSuperset);
  bool IsCollectionSubset(const ConnectionsCollectionVec& collectionSubset, const ConnectionsCollectionVec& collectionSuperset);
  void RemoveRedundantSubsets(std::vector<ConnectionsCollectionVec>& validConnections);
  bool ProvidedConnectionsMatch(ConnectionsCollection collection, ConnectionsList connections);
  StrSet GetValidSets(ContextItem& context, const std::string& clayer);
  void SetDefaultLinkerScript(ContextItem& context);
  void CheckAndGenerateRegionsHeader(ContextItem& context);
  bool GenerateRegionsHeader(ContextItem& context, std::string& generatedRegionsFile);
  void ExpandAccessSequence(const ContextItem& context, const ContextItem& refContext, const std::string& sequence, const std::string& outdir, std::string& item, bool withHeadingDot);
  void ExpandPackDir(ContextItem& context, const std::string& pack, std::string& item);
  bool GetGeneratorDir(const RteGenerator* generator, ContextItem& context, const std::string& layer, std::string& genDir);
  bool GetGeneratorOptions(ContextItem& context, const std::string& layer, GeneratorOptionsItem& options);
  bool GetExtGeneratorOptions(ContextItem& context, const std::string& layer, GeneratorOptionsItem& options);
  bool ParseContextLayers(ContextItem& context);
  bool AddPackRequirements(ContextItem& context, const std::vector<PackItem>& packRequirements);
  void InsertPackRequirements(const std::vector<PackItem>& src, std::vector<PackItem>& dst, std::string base);
  void CheckTypeFilterSpelling(const TypeFilter& typeFilter);
  void CheckCompilerFilterSpelling(const std::string& compiler);
  bool ProcessGeneratedLayers(ContextItem& context);
  void CheckDeviceAttributes(const ContextItem& context, const ProcessorItem& userSelection, const StrMap& targetAttributes);
  std::string GetContextRteFolder(ContextItem& context);
  std::vector<std::string> FindMatchingPackIdsInCbuildPack(const PackItem& needle, const std::vector<ResolvedPackItem>& resolvedPacks);
  void PrintContextErrors(const std::string& contextName);
  void SetFilesDependencies(const GroupNode& group, const std::string& ouput, StrVec& dependsOn, const std::string& dep, const std::string& outDir);
  void SetBuildOutputDependencies(const OutputTypes& outputTypes, const std::string& input, StrVec& dependsOn, const std::string& dep, const std::string& outDir);
  void SetExecutesDependencies(const std::string& output, const std::string& dep, const std::string& outDir);
  void ValidateComponentSources(ContextItem& context);
  void ProcessSelectableCompilers();
  StrVec CollectSelectableCompilers();
  void ProcessTmpDir(std::string& tmpdir, const std::string& base);
  bool IsCreatedByExecute(const std::string file, const std::string dir);
  bool CollectAllRequiredPdscFiles();
  bool ParseTargetSetContextSelection();
  std::vector<RteBoard*> GetCompatibleBoards(const ContextItem& context);
  bool IsBoardListCompatible(const ContextItem& context, const std::vector<RteBoard*> compatibleBoards, const Collection<RteItem*>& boards);
  bool IsEnvironmentCompatible(const std::string& environment, const StrVec& filter);
  bool HasCompatibleEnvironment(const Collection<RteItem*>& environments, const StrVec& filter);
};

#endif  // PROJMGRWORKER_H
