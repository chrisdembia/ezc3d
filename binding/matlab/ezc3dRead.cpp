#include "mex.h"
#include <iostream>
#include <memory>

#include "ezc3d.h"

void fillDoubleInStruct(mxArray *field, mwIndex idx, int value){
    // markers_Number
    mxArray* ptr = mxCreateDoubleMatrix(1, 1, mxREAL);
    double * val = mxGetPr(ptr);
    val[0] = (double)value;
    mxSetFieldByNumber(field, 0, idx, ptr);
}
void fillDoubleInStruct(mxArray *field, mwIndex idx, const std::vector<int>& values){
    // markers_Number
    mxArray* ptr = mxCreateDoubleMatrix(values.size(), 1, mxREAL);
    double * val = mxGetPr(ptr);
    for (int i =0; i < values.size(); ++i)
        val[i] = (double)values[i];
    mxSetFieldByNumber(field, 0, idx, ptr);
}

void fillDoubleInStruct(mxArray *field, mwIndex idx, double value){
    // markers_Number
    mxArray* ptr = mxCreateDoubleMatrix(1, 1, mxREAL);
    double * val = mxGetPr(ptr);
    val[0] = value;
    mxSetFieldByNumber(field, 0, idx, ptr);
}
void fillDoubleInStruct(mxArray *field, mwIndex idx, const std::vector<float>& values){
    // markers_Number
    mxArray* ptr = mxCreateDoubleMatrix(values.size(), 1, mxREAL);
    double * val = mxGetPr(ptr);
    for (int i =0; i < values.size(); ++i)
        val[i] = (double)values[i];
    mxSetFieldByNumber(field, 0, idx, ptr);
}
void fillDoubleInStruct(mxArray *field, mwIndex idx, const std::vector<double>& values){
    // markers_Number
    mxArray* ptr = mxCreateDoubleMatrix(values.size(), 1, mxREAL);
    double * val = mxGetPr(ptr);
    for (int i =0; i < values.size(); ++i)
        val[i] = values[i];
    mxSetFieldByNumber(field, 0, idx, ptr);
}

void fillStrInStruct(mxArray *field, mwIndex idx, const std::string &value){
    // markers_Number
    mxSetFieldByNumber(field, 0, idx, mxCreateString(value.c_str()));
}
void fillStrInStruct(mxArray *field, mwIndex idx, const std::vector<std::string>& values){
    // markers_Number
    mxArray* ptr = mxCreateCellMatrix(values.size(), 1);

    // Fill cell matrix with input arguments
    for (int i =0; i < values.size(); ++i)
        mxSetCell(ptr, i, mxCreateString(values[i].c_str()) );
    mxSetFieldByNumber(field, 0, idx, ptr);
}

void mexFunction(int nlhs,mxArray *plhs[],int nrhs,const mxArray *prhs[])
{
    // Check inputs and outputs
    if (nrhs != 1)
        mexErrMsgTxt("Input argument must be a file path string.");
    if (mxIsChar(prhs[0]) != 1)
        mexErrMsgTxt("Input argument must be a file path string.");
    if (nlhs > 1)
        mexErrMsgTxt("Only one output is available");

    // Receive the path
    char *buffer = mxArrayToString(prhs[0]);
    std::string path(buffer);
    mxFree(buffer);

    // Preparer the first layer of the output structure
    const char *globalFieldsNames[] = {"header", "parameter","data"};
    int headerIdx = 0;
    int parameterIdx = 1;
    int dataIdx = 2;
    mwSize globalFieldsDims[2] = {1, 1};
    plhs[0] = mxCreateStructArray(2, globalFieldsDims, sizeof(globalFieldsNames)/sizeof(*globalFieldsNames), globalFieldsNames);

    // Populate the c3d
    try{
        // Read the c3d
        ezc3d::c3d c3d(path);

        // Fill the header
        {
        const char *headerFieldsNames[] = {"markers", "analogs", "events"};
        mwSize headerFieldsDims[2] = {1, 1};
        mxArray * headerStruct = mxCreateStructArray(2, headerFieldsDims, sizeof(headerFieldsNames)/sizeof(*headerFieldsNames), headerFieldsNames);
        mxSetFieldByNumber(plhs[0], 0, headerIdx, headerStruct);
            // fill markers
            {
                const char *markersFieldsNames[] = {"size", "frameRate", "firstFrame", "lastFrame"};
                mwSize markerFieldsDims[2] = {1, 1};
                mxArray * markersStruct = mxCreateStructArray(2, markerFieldsDims, sizeof(markersFieldsNames)/sizeof(*markersFieldsNames), markersFieldsNames);
                mxSetFieldByNumber(headerStruct, 0, 0, markersStruct);

                fillDoubleInStruct(markersStruct, 0, c3d.header().nb3dPoints());
                fillDoubleInStruct(markersStruct, 1, c3d.header().frameRate());
                fillDoubleInStruct(markersStruct, 2, c3d.header().firstFrame());
                fillDoubleInStruct(markersStruct, 3, c3d.header().lastFrame()-1);
            }
            // fill analogs
            {
                const char *analogsFieldsNames[] = {"size", "frameRate", "firstFrame", "lastFrame"};
                mwSize analogsFieldsDims[2] = {1, 1};
                mxArray * analogsStruct = mxCreateStructArray(2, analogsFieldsDims, sizeof(analogsFieldsNames)/sizeof(*analogsFieldsNames), analogsFieldsNames);
                mxSetFieldByNumber(headerStruct, 0, 1, analogsStruct);

                fillDoubleInStruct(analogsStruct, 0, c3d.header().nbAnalogs());
                fillDoubleInStruct(analogsStruct, 1, c3d.header().nbAnalogByFrame() * c3d.header().frameRate() );
                fillDoubleInStruct(analogsStruct, 2, c3d.header().nbAnalogByFrame() * c3d.header().firstFrame());
                fillDoubleInStruct(analogsStruct, 3, c3d.header().nbAnalogByFrame() * c3d.header().lastFrame()-1); // 0-based
                std::cout <<  c3d.header().nbAnalogByFrame() << std::endl;
                std::cout << c3d.header().lastFrame() << std::endl;
            }

            // fill events
            {
                const char *eventsFieldsNames[] = {"size", "eventsTime", "eventsLabel"};
                mwSize eventsFieldsDims[2] = {1, 1};
                mxArray * eventsStruct = mxCreateStructArray(2, eventsFieldsDims, sizeof(eventsFieldsNames)/sizeof(*eventsFieldsNames), eventsFieldsNames);
                mxSetFieldByNumber(headerStruct, 0, 2, eventsStruct);

                fillDoubleInStruct(eventsStruct, 0, (int)c3d.header().eventsTime().size());
                fillDoubleInStruct(eventsStruct, 1, c3d.header().eventsTime());
                fillStrInStruct(eventsStruct, 2, c3d.header().eventsLabel());
            }
        }


        // Fill the parameters
        {
        const std::vector<ezc3d::ParametersNS::GroupNS::Group> groups = c3d.parameters().groups();
        char **groupsFieldsNames = new char *[groups.size()];
        for (int g = 0; g < groups.size(); ++g){
            groupsFieldsNames[g] = new char[groups[g].name().length() + 1];
            strcpy( groupsFieldsNames[g], groups[g].name().c_str());
        }
        mwSize groupsFieldsDims[2] = {1, 1};
        mxArray * groupsStruct = mxCreateStructArray(2, groupsFieldsDims, groups.size(), (const char**)groupsFieldsNames);
        mxSetFieldByNumber(plhs[0], 0, parameterIdx, groupsStruct);

            // Parse each parameters
            for (int g = 0; g < groups.size(); ++g){
                const std::vector<ezc3d::ParametersNS::GroupNS::Parameter> parameters = groups[g].parameters();
                char **parametersFieldsNames = new char *[parameters.size()];
                for (int p = 0; p < parameters.size(); ++p){
                    parametersFieldsNames[p] = new char[parameters[p].name().length() + 1];
                    strcpy( parametersFieldsNames[p], parameters[p].name().c_str());
                }
                mwSize parametersFieldsDims[2] = {1, 1};
                mxArray * parametersStruct = mxCreateStructArray(2, parametersFieldsDims, parameters.size(), (const char**)parametersFieldsNames);
                mxSetFieldByNumber(groupsStruct, 0, g, parametersStruct);

                // Fill each parameters
                for (int p = 0; p < parameters.size(); ++p){
                    ezc3d::ParametersNS::GroupNS::Parameter param = parameters[p];
                    if (param.type() == ezc3d::ParametersNS::GroupNS::Parameter::DATA_TYPE::INT)
                        fillDoubleInStruct(parametersStruct, p, param.valuesAsInt());
                    else if (param.type() == ezc3d::ParametersNS::GroupNS::Parameter::DATA_TYPE::FLOAT)
                        fillDoubleInStruct(parametersStruct, p, param.valuesAsFloat());
                    else if (param.type() == ezc3d::ParametersNS::GroupNS::Parameter::DATA_TYPE::CHAR)
                        fillStrInStruct(parametersStruct, p, param.valuesAsString());
                }
            }
        }


        // Fill the data
        {
        const char *dataFieldsNames[] = {"points", "analogs"};
        mwSize dataFieldsDims[2] = {1, 1};
        mxArray * dataStruct = mxCreateStructArray(2, dataFieldsDims, 2, dataFieldsNames);
        mxSetFieldByNumber(plhs[0], 0, dataIdx, dataStruct);

            // Fill the point data and analogous data
            {
            mwSize nFramesPoints(c3d.header().lastFrame() - c3d.header().firstFrame());
            mwSize nPoints(c3d.header().nb3dPoints());
            mwSize dataPointsFieldsDims[3] = {3, nPoints, nFramesPoints};
            mxArray * dataPoints = mxCreateNumericArray(3, dataPointsFieldsDims, mxDOUBLE_CLASS, mxREAL);
            double * valPoints = mxGetPr(dataPoints);

            int nFramesAnalogs(nFramesPoints * c3d.header().nbAnalogByFrame());
            int nAnalogs(c3d.header().nbAnalogs());
            int nSubFrames(c3d.header().nbAnalogByFrame());
            mxArray * dataAnalogs = mxCreateDoubleMatrix(nFramesAnalogs, nAnalogs, mxREAL);
            double * valAnalogs = mxGetPr(dataAnalogs);

            for (int f=0; f<nFramesPoints; ++f){
                ezc3d::DataNS::Frame frame(c3d.data().frame(f));
                // Points side
                const std::vector<ezc3d::DataNS::Points3dNS::Point>& points(frame.points().points());
                for (int p=0; p<points.size(); ++p){
                    ezc3d::DataNS::Points3dNS::Point point(points[p]);
                    valPoints[f*nPoints*3+3*p+0] = point.x();
                    valPoints[f*nPoints*3+3*p+1] = point.y();
                    valPoints[f*nPoints*3+3*p+2] = point.z();
                }

                // Analogs side
                const std::vector<ezc3d::DataNS::AnalogsNS::SubFrame>& subframes(frame.analogs().subframes());
                for (int sf=0; sf<subframes.size(); ++sf){
                    const std::vector<ezc3d::DataNS::AnalogsNS::Channel>& channels(subframes[sf].channels());
                    for (int c=0; c<channels.size(); ++c){
                        valAnalogs[c*nSubFrames*nFramesPoints + sf + f*nSubFrames]= channels[c].value();
                    }
                }
            }
            mxSetFieldByNumber(dataStruct, 0, 0, dataPoints);
            mxSetFieldByNumber(dataStruct, 0, 1, dataAnalogs);
            }
        }
    }
    catch (std::string m){
        mexErrMsgTxt(m.c_str());
    }

    return;
}
