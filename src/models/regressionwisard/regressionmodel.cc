class RegressionModel: public Model{
public:
    virtual void train(const BitInput input, const double y) = 0;

    virtual std::vector<double> predict(const DataSet& dataset) const = 0;
    virtual double predict(const BinInput& input) const = 0;
};